#include "temperature_monitor/adc_conversion.hpp"
#include "temperature_monitor/eeprom.hpp"
#include "temperature_monitor/gpio.hpp"
#include "temperature_monitor/i2c.hpp"
#include "temperature_monitor/sampling_timer.hpp"
#include "temperature_monitor/temperature_classification.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>
#include <optional>
#include <sstream>
#include <string>

namespace tempmon = temperature_monitor;

namespace {

constexpr std::size_t mock_eeprom_size{256U};

/*
 * PC mock for the EEPROM memory accessed through the I2C interface.
 */
class MockEepromMemory {
public:
    [[nodiscard]] bool read(
        std::uint8_t device_address,
        std::uint16_t memory_address,
        std::uint8_t* data,
        std::size_t length)
    {
        if ((device_address != tempmon::Eeprom::i2c_address) ||
            (data == nullptr) ||
            !range_is_valid(memory_address, length)) {
            return false;
        }

        const auto offset =
            static_cast<std::size_t>(memory_address);

        std::copy_n(
            memory_.begin() + static_cast<std::ptrdiff_t>(offset),
            length,
            data);

        return true;
    }

    [[nodiscard]] bool write(
        std::uint8_t device_address,
        std::uint16_t memory_address,
        const std::uint8_t* data,
        std::size_t length)
    {
        if ((device_address != tempmon::Eeprom::i2c_address) ||
            (data == nullptr) ||
            !range_is_valid(memory_address, length)) {
            return false;
        }

        const auto offset =
            static_cast<std::size_t>(memory_address);

        std::copy_n(
            data,
            length,
            memory_.begin() + static_cast<std::ptrdiff_t>(offset));

        return true;
    }

private:
    [[nodiscard]] static bool range_is_valid(
        std::uint16_t memory_address,
        std::size_t length) noexcept
    {
        const auto offset =
            static_cast<std::size_t>(memory_address);

        return (offset <= mock_eeprom_size) &&
               (length <= (mock_eeprom_size - offset));
    }

    std::array<std::uint8_t, mock_eeprom_size> memory_{};
};

/*
 * First-order thermal plant controlled by a PI controller.
 *
 * The plant differential equation is:
 *
 *     dT/dt = (Tambient + output - T) / tau
 *
 * The equation is integrated using fourth-order Runge-Kutta.
 */
[[nodiscard]] double simulate_temperature_system(
    double setpoint_c) noexcept
{
    constexpr double ambient_c{0.0};
    constexpr double tau_s{20.0};
    constexpr double proportional_gain{2.0};
    constexpr double integral_gain{0.5};
    constexpr double time_step_s{0.01};
    constexpr double simulation_time_s{200.0};
    constexpr double output_min{-120.0};
    constexpr double output_max{120.0};

    if (!std::isfinite(setpoint_c) ||
        (setpoint_c < 0.0) ||
        (setpoint_c > 120.0)) {
        return std::numeric_limits<double>::quiet_NaN();
    }

    double temperature_c{ambient_c};
    double previous_error{setpoint_c - temperature_c};
    double controller_output{0.0};

    const auto number_of_steps =
        static_cast<std::size_t>(
            simulation_time_s / time_step_s);

    for (std::size_t step{0U};
         step < number_of_steps;
         ++step) {
        const double error{setpoint_c - temperature_c};

        const double candidate_output{
            controller_output +
            (proportional_gain * (error - previous_error)) +
            (integral_gain * time_step_s * error)};

        const double applied_output{
            std::clamp(
                candidate_output,
                output_min,
                output_max)};

        const auto derivative =
            [applied_output](double temperature) noexcept {
                return (
                    ambient_c +
                    applied_output -
                    temperature) /
                    tau_s;
            };

        const double k1{derivative(temperature_c)};

        const double k2{
            derivative(
                temperature_c +
                (0.5 * time_step_s * k1))};

        const double k3{
            derivative(
                temperature_c +
                (0.5 * time_step_s * k2))};

        const double k4{
            derivative(
                temperature_c +
                (time_step_s * k3))};

        temperature_c +=
            (time_step_s / 6.0) *
            (k1 + (2.0 * k2) + (2.0 * k3) + k4);

        controller_output = applied_output;
        previous_error = error;
    }

    return temperature_c;
}

[[nodiscard]] std::optional<tempmon::SensorRevision>
read_hardware_revision()
{
    for (;;) {
        std::cout << "Select hardware revision (A/B): ";

        std::string input;

        if (!std::getline(std::cin, input)) {
            return std::nullopt;
        }

        if (input == "A" ||
            input == "a" ||
            input == "0") {
            return tempmon::SensorRevision::rev_a;
        }

        if (input == "B" ||
            input == "b" ||
            input == "1") {
            return tempmon::SensorRevision::rev_b;
        }

        std::cout << "Invalid revision. Enter A or B.\n";
    }
}

[[nodiscard]] std::optional<double> parse_setpoint(
    const std::string& input)
{
    std::istringstream stream{input};
    double value{0.0};

    if (!(stream >> value)) {
        return std::nullopt;
    }

    stream >> std::ws;

    if (!stream.eof() ||
        !std::isfinite(value) ||
        (value < 0.0) ||
        (value > 120.0)) {
        return std::nullopt;
    }

    return value;
}

[[nodiscard]] char revision_letter(
    tempmon::SensorRevision revision) noexcept
{
    return revision == tempmon::SensorRevision::rev_a
        ? 'A'
        : 'B';
}

void print_traffic_light(const tempmon::LedState& state)
{
    std::cout << "\nTraffic light\n"
              << "  RED    ["
              << (state.red ? "ON " : "OFF")
              << "]\n"
              << "  YELLOW ["
              << (state.yellow ? "ON " : "OFF")
              << "]\n"
              << "  GREEN  ["
              << (state.green ? "ON " : "OFF")
              << "]\n";
}

} // namespace

int main()
{
    MockEepromMemory mock_eeprom;

    const tempmon::I2c i2c{
        [&mock_eeprom](
            std::uint8_t device_address,
            std::uint16_t memory_address,
            std::uint8_t* data,
            std::size_t length) {
            return mock_eeprom.read(
                device_address,
                memory_address,
                data,
                length);
        },
        [&mock_eeprom](
            std::uint8_t device_address,
            std::uint16_t memory_address,
            const std::uint8_t* data,
            std::size_t length) {
            return mock_eeprom.write(
                device_address,
                memory_address,
                data,
                length);
        }};

    const tempmon::Eeprom eeprom{i2c};

    std::cout << "Temperature Monitor\n";
    std::cout << "-------------------\n";

    const auto selected_revision = read_hardware_revision();

    if (!selected_revision.has_value()) {
        std::cerr << "Failed to read hardware revision.\n";
        return EXIT_FAILURE;
    }

    const tempmon::EepromConfig stored_config{
        *selected_revision,
        "ABC1234"
    };

    if (!eeprom.write_config(stored_config)) {
        std::cerr << "Failed to initialize mocked EEPROM.\n";
        return EXIT_FAILURE;
    }

    const auto loaded_config = eeprom.read_config();

    if (!loaded_config.has_value()) {
        std::cerr << "Failed to read EEPROM configuration.\n";
        return EXIT_FAILURE;
    }

    const auto* const sensor =
        tempmon::sensor_config(
            loaded_config->hardware_revision);

    if (sensor == nullptr) {
        std::cerr << "Unsupported hardware revision.\n";
        return EXIT_FAILURE;
    }

    const tempmon::Gpio gpio{
        [](const tempmon::LedState& state) {
            print_traffic_light(state);
        }};

    std::uint32_t adc_digit{0U};

    tempmon::SamplingTimer sampling_timer{
        [&adc_digit, sensor, &gpio]() {
            const auto measured_temperature =
                tempmon::adc_to_temperature(
                    adc_digit,
                    *sensor);

            const auto status =
                measured_temperature.has_value()
                    ? tempmon::classify_temperature(
                          *measured_temperature)
                    : tempmon::TemperatureStatus::invalid;

            std::cout << "ADC digit:          "
                      << adc_digit
                      << '\n';

            std::cout << "Measured:           ";

            if (measured_temperature.has_value()) {
                std::cout
                    << std::fixed
                    << std::setprecision(1)
                    << *measured_temperature
                    << " C\n";
            } else {
                std::cout << "nan C\n";
            }

            std::cout << "Condition:          "
                      << tempmon::temperature_status_name(status)
                      << '\n';

            gpio.set_temperature_status(status);
        }};

    sampling_timer.start();

    std::cout << "\nSerial number:      "
              << loaded_config->serial_number
              << '\n';

    std::cout << "Hardware revision:  Rev-"
              << revision_letter(
                     loaded_config->hardware_revision)
              << '\n';

    std::cout << "Sensor resolution:  "
              << std::fixed
              << std::setprecision(1)
              << sensor->degrees_c_per_digit
              << " C/digit\n";

    std::cout << "Sampling period:    "
              << tempmon::SamplingTimer::sample_period.count()
              << " us\n";

    std::cout << "Valid setpoints:    0 to 120 C\n";

    for (;;) {
        std::cout
            << "\nEnter setpoint in C, or q to quit: ";

        std::string input;

        if (!std::getline(std::cin, input)) {
            break;
        }

        if ((input == "q") || (input == "Q")) {
            break;
        }

        const auto setpoint_c = parse_setpoint(input);

        if (!setpoint_c.has_value()) {
            std::cout
                << "Invalid input. Enter a value from 0 to 120.\n";
            continue;
        }

        const double simulated_temperature_c{
            simulate_temperature_system(*setpoint_c)};

        if (!std::isfinite(simulated_temperature_c)) {
            std::cerr << "Temperature simulation failed.\n";
            continue;
        }

        const auto converted_adc_digit =
            tempmon::temperature_to_adc(
                simulated_temperature_c,
                *sensor);

        if (!converted_adc_digit.has_value()) {
            std::cerr << "ADC conversion failed.\n";
            continue;
        }

        adc_digit = *converted_adc_digit;

        std::cout << "\nSetpoint:           "
                  << std::fixed
                  << std::setprecision(1)
                  << *setpoint_c
                  << " C\n";

        std::cout << "Simulated plant:    "
                  << std::fixed
                  << std::setprecision(3)
                  << simulated_temperature_c
                  << " C\n";

        /*
         * Simulate one 100 us hardware timer interrupt.
         *
         * The timer dispatches the complete monitoring pipeline:
         *
         * ADC digit
         *   -> measured temperature
         *   -> classification
         *   -> GPIO traffic light
         */
        sampling_timer.irq_handler();
    }

    sampling_timer.stop();

    std::cout << "\nTemperature Monitor stopped.\n";
    return EXIT_SUCCESS;
}
