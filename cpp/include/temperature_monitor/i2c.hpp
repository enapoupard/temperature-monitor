#ifndef TEMPERATURE_MONITOR_I2C_HPP
#define TEMPERATURE_MONITOR_I2C_HPP

#include <cstddef>
#include <cstdint>
#include <functional>

namespace temperature_monitor {

class I2c {
public:
    using ReadFunction = std::function<bool(
        std::uint8_t device_address,
        std::uint16_t memory_address,
        std::uint8_t* data,
        std::size_t length)>;

    using WriteFunction = std::function<bool(
        std::uint8_t device_address,
        std::uint16_t memory_address,
        const std::uint8_t* data,
        std::size_t length)>;

    I2c(
        ReadFunction read_function,
        WriteFunction write_function);

    [[nodiscard]] bool read(
        std::uint8_t device_address,
        std::uint16_t memory_address,
        std::uint8_t* data,
        std::size_t length) const;

    [[nodiscard]] bool write(
        std::uint8_t device_address,
        std::uint16_t memory_address,
        const std::uint8_t* data,
        std::size_t length) const;

private:
    ReadFunction read_function_;
    WriteFunction write_function_;
};

} // namespace temperature_monitor

#endif
