#include "temperature_monitor/gpio.hpp"

#include <stdexcept>
#include <utility>

namespace temperature_monitor {

Gpio::Gpio(WriteFunction write_function)
    : write_function_{std::move(write_function)}
{
    if (!write_function_) {
        throw std::invalid_argument{
            "Gpio requires a valid write function"};
    }
}

void Gpio::set_temperature_status(TemperatureStatus status) const
{
    LedState state{};

    switch (status) {
    case TemperatureStatus::green:
        state.green = true;
        break;

    case TemperatureStatus::yellow:
        state.yellow = true;
        break;

    case TemperatureStatus::red:
    case TemperatureStatus::invalid:
    default:
        /*
         * Invalid and unknown states activate red as a fail-safe.
         */
        state.red = true;
        break;
    }

    write_function_(state);
}

} // namespace temperature_monitor
