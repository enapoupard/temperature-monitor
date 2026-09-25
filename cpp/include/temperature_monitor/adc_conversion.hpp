#ifndef TEMPERATURE_MONITOR_ADC_CONVERSION_HPP
#define TEMPERATURE_MONITOR_ADC_CONVERSION_HPP

#include <cstdint>
#include <optional>

namespace temperature_monitor {

enum class SensorRevision : std::uint8_t {
    rev_a = 0,
    rev_b = 1
};

struct SensorConfig {
    SensorRevision revision;
    double degrees_c_per_digit;
};

[[nodiscard]] const SensorConfig* sensor_config(
    SensorRevision revision) noexcept;

[[nodiscard]] std::optional<double> adc_to_temperature(
    std::uint32_t adc_digit,
    const SensorConfig& config) noexcept;

[[nodiscard]] std::optional<std::uint32_t> temperature_to_adc(
    double temperature_c,
    const SensorConfig& config) noexcept;

} // namespace temperature_monitor

#endif
