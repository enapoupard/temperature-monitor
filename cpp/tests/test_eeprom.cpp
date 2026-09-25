#include "temperature_monitor/eeprom.hpp"
#include "temperature_monitor/i2c.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <stdexcept>

namespace tempmon = temperature_monitor;

namespace {

constexpr std::size_t mock_eeprom_size{256U};

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

    void set_revision_byte(std::uint8_t value)
    {
        memory_[tempmon::Eeprom::revision_address] = value;
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

tempmon::I2c make_i2c(MockEepromMemory& memory)
{
    return tempmon::I2c{
        [&memory](
            std::uint8_t device_address,
            std::uint16_t memory_address,
            std::uint8_t* data,
            std::size_t length) {
            return memory.read(
                device_address,
                memory_address,
                data,
                length);
        },
        [&memory](
            std::uint8_t device_address,
            std::uint16_t memory_address,
            const std::uint8_t* data,
            std::size_t length) {
            return memory.write(
                device_address,
                memory_address,
                data,
                length);
        }};
}

void test_write_and_read_rev_a()
{
    MockEepromMemory memory;
    const auto i2c = make_i2c(memory);
    const tempmon::Eeprom eeprom{i2c};

    const tempmon::EepromConfig written{
        tempmon::SensorRevision::rev_a,
        "ABC1234"
    };

    assert(eeprom.write_config(written));

    const auto read = eeprom.read_config();

    assert(read.has_value());
    assert(read->hardware_revision ==
           tempmon::SensorRevision::rev_a);
    assert(read->serial_number == "ABC1234");
}

void test_write_and_read_rev_b()
{
    MockEepromMemory memory;
    const auto i2c = make_i2c(memory);
    const tempmon::Eeprom eeprom{i2c};

    const tempmon::EepromConfig written{
        tempmon::SensorRevision::rev_b,
        "XYZ9876"
    };

    assert(eeprom.write_config(written));

    const auto read = eeprom.read_config();

    assert(read.has_value());
    assert(read->hardware_revision ==
           tempmon::SensorRevision::rev_b);
    assert(read->serial_number == "XYZ9876");
}

void test_invalid_revision_is_rejected()
{
    MockEepromMemory memory;
    const auto i2c = make_i2c(memory);
    const tempmon::Eeprom eeprom{i2c};

    const tempmon::EepromConfig invalid{
        static_cast<tempmon::SensorRevision>(2),
        "ABC1234"
    };

    assert(!eeprom.write_config(invalid));

    memory.set_revision_byte(2U);

    assert(!eeprom.read_config().has_value());
}

void test_invalid_serial_length_is_rejected()
{
    MockEepromMemory memory;
    const auto i2c = make_i2c(memory);
    const tempmon::Eeprom eeprom{i2c};

    const tempmon::EepromConfig too_short{
        tempmon::SensorRevision::rev_a,
        "ABC"
    };

    const tempmon::EepromConfig too_long{
        tempmon::SensorRevision::rev_b,
        "ABC12345"
    };

    assert(!eeprom.write_config(too_short));
    assert(!eeprom.write_config(too_long));
}

void test_i2c_rejects_invalid_buffers()
{
    MockEepromMemory memory;
    const auto i2c = make_i2c(memory);

    std::uint8_t byte{0U};

    assert(!i2c.read(
        tempmon::Eeprom::i2c_address,
        0U,
        nullptr,
        1U));

    assert(!i2c.read(
        tempmon::Eeprom::i2c_address,
        0U,
        &byte,
        0U));

    assert(!i2c.write(
        tempmon::Eeprom::i2c_address,
        0U,
        nullptr,
        1U));

    assert(!i2c.write(
        tempmon::Eeprom::i2c_address,
        0U,
        &byte,
        0U));
}

void test_i2c_rejects_empty_functions()
{
    bool exception_was_thrown{false};

    try {
        tempmon::I2c i2c{
            tempmon::I2c::ReadFunction{},
            tempmon::I2c::WriteFunction{}};
        (void)i2c;
    } catch (const std::invalid_argument&) {
        exception_was_thrown = true;
    }

    assert(exception_was_thrown);
}

void test_wrong_device_address_and_out_of_range_access()
{
    MockEepromMemory memory;
    const auto i2c = make_i2c(memory);
    std::uint8_t byte{0U};

    assert(!i2c.read(
        0x51U,
        0U,
        &byte,
        1U));

    assert(!i2c.write(
        0x51U,
        0U,
        &byte,
        1U));

    assert(!i2c.read(
        tempmon::Eeprom::i2c_address,
        255U,
        &byte,
        2U));

    assert(!i2c.write(
        tempmon::Eeprom::i2c_address,
        255U,
        &byte,
        2U));
}

} // namespace

int main()
{
    test_write_and_read_rev_a();
    test_write_and_read_rev_b();
    test_invalid_revision_is_rejected();
    test_invalid_serial_length_is_rejected();
    test_i2c_rejects_invalid_buffers();
    test_i2c_rejects_empty_functions();
    test_wrong_device_address_and_out_of_range_access();

    std::cout << "C++ I2C and EEPROM tests passed.\n";
    return 0;
}
