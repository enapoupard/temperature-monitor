#include "temperature_monitor/eeprom.hpp"

#include <array>
#include <cstdint>

namespace temperature_monitor {
namespace {

[[nodiscard]] bool revision_is_valid(
    SensorRevision revision) noexcept
{
    return (revision == SensorRevision::rev_a) ||
           (revision == SensorRevision::rev_b);
}

} // namespace

Eeprom::Eeprom(const I2c& i2c) noexcept
    : i2c_{i2c}
{
}

std::optional<EepromConfig> Eeprom::read_config() const
{
    std::uint8_t revision_value{0U};

    if (!i2c_.read(
            i2c_address,
            revision_address,
            &revision_value,
            1U)) {
        return std::nullopt;
    }

    const auto revision =
        static_cast<SensorRevision>(revision_value);

    if (!revision_is_valid(revision)) {
        return std::nullopt;
    }

    std::array<std::uint8_t, serial_length> serial_bytes{};

    if (!i2c_.read(
            i2c_address,
            serial_address,
            serial_bytes.data(),
            serial_bytes.size())) {
        return std::nullopt;
    }

    EepromConfig config{
        revision,
        std::string{
            serial_bytes.begin(),
            serial_bytes.end()}
    };

    return config;
}

bool Eeprom::write_config(const EepromConfig& config) const
{
    if (!revision_is_valid(config.hardware_revision) ||
        (config.serial_number.size() != serial_length)) {
        return false;
    }

    const auto revision_value =
        static_cast<std::uint8_t>(config.hardware_revision);

    if (!i2c_.write(
            i2c_address,
            revision_address,
            &revision_value,
            1U)) {
        return false;
    }

    return i2c_.write(
        i2c_address,
        serial_address,
        reinterpret_cast<const std::uint8_t*>(
            config.serial_number.data()),
        config.serial_number.size());
}

} // namespace temperature_monitor
