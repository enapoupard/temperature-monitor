#include "temperature_monitor/adc_conversion.hpp"

#include <cmath>
#include <cstdint>
#include <limits>

namespace temperature_monitor {
namespace {

constexpr SensorConfig rev_a_config {
    SensorRevision::rev_a,
    1.0
};

constexpr SensorConfig rev_b_config {
    SensorRevision::rev_b,
    0.1
};

[[nodiscard]] bool config_is_valid(
    const SensorConfig& config) noexcept
{
    const bool revision_is_valid =
        (config.revision == SensorRevision::rev_a) ||
        (config.revision == SensorRevision::rev_b);

    return revision_is_valid &&
           std::isfinite(config.degrees_c_per_digit) &&
           (config.degrees_c_per_digit > 0.0);
}

} // namespace

const SensorConfig* sensor_config(
    SensorRevision revision) noexcept
{
    switch (revision) {
    case SensorRevision::rev_a:
        return &rev_a_config;

    case SensorRevision::rev_b:
        return &rev_b_config;

    default:
        return nullptr;
    }
}

std::optional<double> adc_to_temperature(
    std::uint32_t adc_digit,
    const SensorConfig& config) noexcept
{
    if (!config_is_valid(config)) {
        return std::nullopt;
    }

    return static_cast<double>(adc_digit) *
           config.degrees_c_per_digit;
}

std::optional<std::uint32_t> temperature_to_adc(
    double temperature_c,
    const SensorConfig& config) noexcept
{
    if (!config_is_valid(config) ||
        !std::isfinite(temperature_c) ||
        (temperature_c < 0.0)) {
        return std::nullopt;
    }

    const double digit =
        temperature_c / config.degrees_c_per_digit;

    constexpr auto maximum_digit =
        std::numeric_limits<std::uint32_t>::max();

    if (digit > static_cast<double>(maximum_digit)) {
        return std::nullopt;
    }

    return static_cast<std::uint32_t>(
        std::floor(digit + 0.5));
}

} // namespace temperature_monitor
