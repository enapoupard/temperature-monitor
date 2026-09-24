#include "temperature_monitor/eeprom.h"
#include "temperature_monitor/i2c.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define MOCK_EEPROM_SIZE (256U)

typedef struct {
    uint8_t memory[MOCK_EEPROM_SIZE];
} mock_eeprom_t;

static int mock_i2c_read(
    uint8_t device_address,
    uint16_t memory_address,
    uint8_t *data,
    size_t length,
    void *context)
{
    mock_eeprom_t *mock = context;

    if ((device_address != EEPROM_I2C_ADDRESS) ||
        (mock == NULL) ||
        ((size_t)memory_address + length > MOCK_EEPROM_SIZE)) {
        return -1;
    }

    memcpy(data, &mock->memory[memory_address], length);
    return 0;
}

static int mock_i2c_write(
    uint8_t device_address,
    uint16_t memory_address,
    const uint8_t *data,
    size_t length,
    void *context)
{
    mock_eeprom_t *mock = context;

    if ((device_address != EEPROM_I2C_ADDRESS) ||
        (mock == NULL) ||
        ((size_t)memory_address + length > MOCK_EEPROM_SIZE)) {
        return -1;
    }

    memcpy(&mock->memory[memory_address], data, length);
    return 0;
}

int main(void)
{
    mock_eeprom_t mock = { { 0U } };
    i2c_t i2c;
    eeprom_config_t written = {
        .hardware_revision = SENSOR_REV_B,
        .serial_number = "ABC1234"
    };
    eeprom_config_t read = {
        .hardware_revision = SENSOR_REV_A,
        .serial_number = { '\0' }
    };

    assert(i2c_init(
               &i2c,
               mock_i2c_read,
               mock_i2c_write,
               &mock) == 0);

    assert(eeprom_write_config(&i2c, &written) == 0);
    assert(eeprom_read_config(&i2c, &read) == 0);

    assert(read.hardware_revision == SENSOR_REV_B);
    assert(strcmp(read.serial_number, "ABC1234") == 0);

    mock.memory[EEPROM_REVISION_ADDRESS] = 2U;
    assert(eeprom_read_config(&i2c, &read) == -1);

    assert(eeprom_read_config(NULL, &read) == -1);
    assert(eeprom_read_config(&i2c, NULL) == -1);
    assert(eeprom_write_config(NULL, &written) == -1);
    assert(eeprom_write_config(&i2c, NULL) == -1);

    puts("I2C and EEPROM tests passed.");
    return 0;
}
