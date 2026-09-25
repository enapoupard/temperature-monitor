#include "temperature_monitor/i2c.hpp"

#include <stdexcept>
#include <utility>

namespace temperature_monitor {

I2c::I2c(
    ReadFunction read_function,
    WriteFunction write_function)
    : read_function_{std::move(read_function)},
      write_function_{std::move(write_function)}
{
    if (!read_function_ || !write_function_) {
        throw std::invalid_argument{
            "I2c requires valid read and write functions"};
    }
}

bool I2c::read(
    std::uint8_t device_address,
    std::uint16_t memory_address,
    std::uint8_t* data,
    std::size_t length) const
{
    if ((data == nullptr) || (length == 0U)) {
        return false;
    }

    return read_function_(
        device_address,
        memory_address,
        data,
        length);
}

bool I2c::write(
    std::uint8_t device_address,
    std::uint16_t memory_address,
    const std::uint8_t* data,
    std::size_t length) const
{
    if ((data == nullptr) || (length == 0U)) {
        return false;
    }

    return write_function_(
        device_address,
        memory_address,
        data,
        length);
}

} // namespace temperature_monitor
