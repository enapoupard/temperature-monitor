#include "temperature_monitor/eeprom.h"

#include <stddef.h>
#include <stdint.h>

int eeprom_read_config(
    const i2c_t *i2c,
    eeprom_config_t *config)
{
    uint8_t revision;

    if ((i2c == NULL) || (config == NULL)) {
        return -1;
    }

    if (i2c_read(
            i2c,
            EEPROM_I2C_ADDRESS,
            EEPROM_REVISION_ADDRESS,
            &revision,
            1U) != 0) {
        return -1;
    }

    if ((revision != (uint8_t)SENSOR_REV_A) &&
        (revision != (uint8_t)SENSOR_REV_B)) {
        return -1;
    }

    if (i2c_read(
            i2c,
            EEPROM_I2C_ADDRESS,
            EEPROM_SERIAL_ADDRESS,
            (uint8_t *)config->serial_number,
            EEPROM_SERIAL_LENGTH) != 0) {
        return -1;
    }

    config->hardware_revision = (sensor_revision_t)revision;
    config->serial_number[EEPROM_SERIAL_LENGTH] = '\0';

    return 0;
}

int eeprom_write_config(
    const i2c_t *i2c,
    const eeprom_config_t *config)
{
    uint8_t revision;

    if ((i2c == NULL) || (config == NULL)) {
        return -1;
    }

    if ((config->hardware_revision != SENSOR_REV_A) &&
        (config->hardware_revision != SENSOR_REV_B)) {
        return -1;
    }

    revision = (uint8_t)config->hardware_revision;

    if (i2c_write(
            i2c,
            EEPROM_I2C_ADDRESS,
            EEPROM_REVISION_ADDRESS,
            &revision,
            1U) != 0) {
        return -1;
    }

    if (i2c_write(
            i2c,
            EEPROM_I2C_ADDRESS,
            EEPROM_SERIAL_ADDRESS,
            (const uint8_t *)config->serial_number,
            EEPROM_SERIAL_LENGTH) != 0) {
        return -1;
    }

    return 0;
}
