#ifndef TEMPERATURE_MONITOR_EEPROM_HPP
#define TEMPERATURE_MONITOR_EEPROM_HPP

#include "temperature_monitor/adc_conversion.hpp"
#include "temperature_monitor/i2c.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>

namespace temperature_monitor {

struct EepromConfig {
    SensorRevision hardware_revision;
    std::string serial_number;
};

class Eeprom {
public:
    static constexpr std::uint8_t i2c_address{0x50U};
    static constexpr std::uint16_t revision_address{0U};
    static constexpr std::uint16_t serial_address{1U};
    static constexpr std::size_t serial_length{7U};

    explicit Eeprom(const I2c& i2c) noexcept;

    [[nodiscard]] std::optional<EepromConfig> read_config() const;

    [[nodiscard]] bool write_config(
        const EepromConfig& config) const;

private:
    const I2c& i2c_;
};

} // namespace temperature_monitor

#endif
