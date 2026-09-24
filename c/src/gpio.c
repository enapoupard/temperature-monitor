#include "temperature_monitor/gpio.h"

#include <stddef.h>

int gpio_init(
    gpio_t *gpio,
    gpio_write_function_t write_function,
    void *context)
{
    if ((gpio == NULL) || (write_function == NULL)) {
        return -1;
    }

    gpio->write = write_function;
    gpio->context = context;

    return 0;
}

void gpio_set_temperature_status(
    const gpio_t *gpio,
    temperature_status_t status)
{
    gpio_led_state_t state = {
        .green = 0,
        .yellow = 0,
        .red = 0
    };

    if ((gpio == NULL) || (gpio->write == NULL)) {
        return;
    }

    switch (status) {
    case TEMPERATURE_STATUS_GREEN:
        state.green = 1;
        break;

    case TEMPERATURE_STATUS_YELLOW:
        state.yellow = 1;
        break;

    case TEMPERATURE_STATUS_RED:
    case TEMPERATURE_STATUS_INVALID:
    default:
        /*
         * Invalid or unknown states activate the red LED as a fail-safe.
         */
        state.red = 1;
        break;
    }

    gpio->write(&state, gpio->context);
}
