#include "temperature_monitor/adc_conversion.h"
#include "temperature_monitor/eeprom.h"
#include "temperature_monitor/gpio.h"
#include "temperature_monitor/i2c.h"
#include "temperature_monitor/sampling_timer.h"
#include "temperature_monitor/sensor_config.h"
#include "temperature_monitor/temperature_classification.h"

#include <errno.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MOCK_EEPROM_SIZE (256U)

typedef struct {
    uint8_t memory[MOCK_EEPROM_SIZE];
} mock_eeprom_t;

typedef struct {
    uint32_t adc_digit;
    const sensor_config_t *sensor_config;
    const gpio_t *gpio;
} monitor_context_t;

static double simulate_temperature_system(double setpoint_c)
{
    const double ambient_c = 0.0;
    const double tau_s = 20.0;
    const double kp = 2.0;
    const double ki = 0.5;
    const double dt_s = 0.01;
    const double simulation_time_s = 200.0;
    const double output_min = -120.0;
    const double output_max = 120.0;

    double temperature_c = ambient_c;
    double previous_error = setpoint_c - temperature_c;
    double output = 0.0;
    const int steps = (int)(simulation_time_s / dt_s);
    int i;

    if (!isfinite(setpoint_c) ||
        (setpoint_c < 0.0) ||
        (setpoint_c > 120.0)) {
        return NAN;
    }

    for (i = 0; i < steps; ++i) {
        const double error = setpoint_c - temperature_c;
        const double candidate =
            output +
            (kp * (error - previous_error)) +
            (ki * dt_s * error);

        double applied = candidate;
        double k1;
        double k2;
        double k3;
        double k4;

        if (applied > output_max) {
            applied = output_max;
        } else if (applied < output_min) {
            applied = output_min;
        }

        k1 = (ambient_c + applied - temperature_c) / tau_s;

        k2 = (ambient_c + applied -
              (temperature_c + (0.5 * dt_s * k1))) / tau_s;

        k3 = (ambient_c + applied -
              (temperature_c + (0.5 * dt_s * k2))) / tau_s;

        k4 = (ambient_c + applied -
              (temperature_c + (dt_s * k3))) / tau_s;

        temperature_c +=
            (dt_s / 6.0) *
            (k1 + (2.0 * k2) + (2.0 * k3) + k4);

        output = applied;
        previous_error = error;
    }

    return temperature_c;
}

static int mock_i2c_read(
    uint8_t device_address,
    uint16_t memory_address,
    uint8_t *data,
    size_t length,
    void *context)
{
    mock_eeprom_t *eeprom = context;

    if ((device_address != EEPROM_I2C_ADDRESS) ||
        (eeprom == NULL) ||
        (data == NULL) ||
        ((size_t)memory_address + length > MOCK_EEPROM_SIZE)) {
        return -1;
    }

    memcpy(data, &eeprom->memory[memory_address], length);
    return 0;
}

static int mock_i2c_write(
    uint8_t device_address,
    uint16_t memory_address,
    const uint8_t *data,
    size_t length,
    void *context)
{
    mock_eeprom_t *eeprom = context;

    if ((device_address != EEPROM_I2C_ADDRESS) ||
        (eeprom == NULL) ||
        (data == NULL) ||
        ((size_t)memory_address + length > MOCK_EEPROM_SIZE)) {
        return -1;
    }

    memcpy(&eeprom->memory[memory_address], data, length);
    return 0;
}

static void print_leds(
    const gpio_led_state_t *state,
    void *context)
{
    (void)context;

    printf("\nTraffic light\n");
    printf("  RED    [%s]\n", state->red != 0 ? "ON " : "OFF");
    printf("  YELLOW [%s]\n", state->yellow != 0 ? "ON " : "OFF");
    printf("  GREEN  [%s]\n", state->green != 0 ? "ON " : "OFF");
}

static const char *status_to_string(temperature_status_t status)
{
    switch (status) {
    case TEMPERATURE_STATUS_GREEN:
        return "NORMAL";

    case TEMPERATURE_STATUS_YELLOW:
        return "WARNING";

    case TEMPERATURE_STATUS_RED:
        return "CRITICAL";

    case TEMPERATURE_STATUS_INVALID:
    default:
        return "INVALID";
    }
}

static void temperature_sample_callback(void *context)
{
    monitor_context_t *monitor = context;
    temperature_status_t status;
    double measured_temperature_c;

    if ((monitor == NULL) ||
        (monitor->sensor_config == NULL) ||
        (monitor->gpio == NULL)) {
        return;
    }

    if (adc_to_temperature(
            monitor->adc_digit,
            monitor->sensor_config,
            &measured_temperature_c) != ADC_CONVERSION_OK) {
        status = TEMPERATURE_STATUS_INVALID;
        measured_temperature_c = NAN;
    } else {
        status = temperature_classify(measured_temperature_c);
    }

    printf("ADC digit:          %u\n",
           (unsigned int)monitor->adc_digit);
    printf("Measured:           %.1f C\n", measured_temperature_c);
    printf("Condition:          %s\n", status_to_string(status));

    gpio_set_temperature_status(monitor->gpio, status);
}

static int parse_setpoint(
    const char *text,
    double *setpoint_c)
{
    char *end;
    double value;

    if ((text == NULL) || (setpoint_c == NULL)) {
        return -1;
    }

    errno = 0;
    value = strtod(text, &end);

    if ((end == text) || (errno != 0) || !isfinite(value)) {
        return -1;
    }

    while ((*end == ' ') || (*end == '\t')) {
        ++end;
    }

    if ((*end != '\n') && (*end != '\0')) {
        return -1;
    }

    if ((value < 0.0) || (value > 120.0)) {
        return -1;
    }

    *setpoint_c = value;
    return 0;
}

int main(void)
{
    mock_eeprom_t mock_eeprom = { { 0U } };
    eeprom_config_t stored_config = {
        .hardware_revision = SENSOR_REV_B,
        .serial_number = "ABC1234"
    };

    eeprom_config_t loaded_config;
    i2c_t i2c;
    gpio_t gpio;
    monitor_context_t monitor;
    char input[64];

    if (i2c_init(
            &i2c,
            mock_i2c_read,
            mock_i2c_write,
            &mock_eeprom) != 0) {
        fprintf(stderr, "Failed to initialize mocked I2C.\n");
        return EXIT_FAILURE;
    }

    printf("Temperature Monitor\n");
    printf("-------------------\n");

    for (;;) {
        printf("Select hardware revision (A/B): ");

        if (fgets(input, sizeof(input), stdin) == NULL) {
            fprintf(stderr, "Failed to read hardware revision.\n");
            return EXIT_FAILURE;
        }

        if ((input[0] == 'A') ||
            (input[0] == 'a') ||
            (input[0] == '0')) {
            stored_config.hardware_revision = SENSOR_REV_A;
            break;
        }

        if ((input[0] == 'B') ||
            (input[0] == 'b') ||
            (input[0] == '1')) {
            stored_config.hardware_revision = SENSOR_REV_B;
            break;
        }

        printf("Invalid revision. Enter A or B.\n");
    }

    if (eeprom_write_config(&i2c, &stored_config) != 0) {
        fprintf(stderr, "Failed to initialize mocked EEPROM.\n");
        return EXIT_FAILURE;
    }

    if (eeprom_read_config(&i2c, &loaded_config) != 0) {
        fprintf(stderr, "Failed to read EEPROM configuration.\n");
        return EXIT_FAILURE;
    }

    if (gpio_init(&gpio, print_leds, NULL) != 0) {
        fprintf(stderr, "Failed to initialize mocked GPIO.\n");
        return EXIT_FAILURE;
    }

    monitor.adc_digit = 0U;
    monitor.sensor_config =
        sensor_config_get(loaded_config.hardware_revision);
    monitor.gpio = &gpio;

    if (monitor.sensor_config == NULL) {
        fprintf(stderr, "Unsupported hardware revision.\n");
        return EXIT_FAILURE;
    }

    if (sampling_timer_init(
            TEMPERATURE_SAMPLE_PERIOD_US,
            temperature_sample_callback,
            &monitor) != 0) {
        fprintf(stderr, "Failed to initialize sampling timer.\n");
        return EXIT_FAILURE;
    }

    sampling_timer_start();

    printf("\nSerial number:      %s\n", loaded_config.serial_number);
    printf("Hardware revision:  Rev-%c\n",
           loaded_config.hardware_revision == SENSOR_REV_A ? 'A' : 'B');
    printf("Sensor resolution:  %.1f C/digit\n",
           monitor.sensor_config->degrees_c_per_digit);
    printf("Sampling period:    %u us\n",
           TEMPERATURE_SAMPLE_PERIOD_US);
    printf("Valid setpoints:    0 to 120 C\n");

    for (;;) {
        double setpoint_c;
        double simulated_temperature_c;

        printf("\nEnter setpoint in C, or q to quit: ");

        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        if ((input[0] == 'q') || (input[0] == 'Q')) {
            break;
        }

        if (parse_setpoint(input, &setpoint_c) != 0) {
            printf("Invalid input. Enter a value from 0 to 120.\n");
            continue;
        }

        simulated_temperature_c =
            simulate_temperature_system(setpoint_c);

        if (!isfinite(simulated_temperature_c)) {
            fprintf(stderr, "Temperature simulation failed.\n");
            continue;
        }

        if (temperature_to_adc(
                simulated_temperature_c,
                monitor.sensor_config,
                &monitor.adc_digit) != ADC_CONVERSION_OK) {
            fprintf(stderr, "ADC conversion failed.\n");
            continue;
        }

        printf("\nSetpoint:           %.1f C\n", setpoint_c);
        printf("Simulated plant:    %.3f C\n",
               simulated_temperature_c);

        /*
         * Simulate one 100 us hardware-timer interrupt. The same dispatcher
         * would be called by a timer ISR on a real microcontroller.
         */
        sampling_timer_irq_handler();
    }

    sampling_timer_stop();

    printf("\nTemperature Monitor stopped.\n");
    return EXIT_SUCCESS;
}
