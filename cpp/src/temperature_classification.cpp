#include "temperature_monitor/temperature_classification.hpp"

#include <cmath>

namespace temperature_monitor {

TemperatureStatus classify_temperature(double temperature_c) noexcept
{
    if (!std::isfinite(temperature_c)) {
        return TemperatureStatus::invalid;
    }

    if ((temperature_c < 5.0) || (temperature_c >= 105.0)) {
        return TemperatureStatus::red;
    }

    if (temperature_c >= 85.0) {
        return TemperatureStatus::yellow;
    }

    return TemperatureStatus::green;
}

const char* temperature_status_name(TemperatureStatus status) noexcept
{
    switch (status) {
    case TemperatureStatus::green:
        return "NORMAL";

    case TemperatureStatus::yellow:
        return "WARNING";

    case TemperatureStatus::red:
        return "CRITICAL";

    case TemperatureStatus::invalid:
        return "INVALID";

    default:
        return "INVALID";
    }
}

} // namespace temperature_monitor
