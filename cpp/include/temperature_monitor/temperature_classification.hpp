#ifndef TEMPERATURE_MONITOR_TEMPERATURE_CLASSIFICATION_HPP
#define TEMPERATURE_MONITOR_TEMPERATURE_CLASSIFICATION_HPP

namespace temperature_monitor {

enum class TemperatureStatus {
    green,
    yellow,
    red,
    invalid
};

[[nodiscard]] TemperatureStatus classify_temperature(
    double temperature_c) noexcept;

[[nodiscard]] const char* temperature_status_name(
    TemperatureStatus status) noexcept;

} // namespace temperature_monitor

#endif
