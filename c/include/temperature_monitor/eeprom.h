#ifndef TEMPERATURE_MONITOR_EEPROM_H
#define TEMPERATURE_MONITOR_EEPROM_H

#include <stdint.h>

#include "temperature_monitor/i2c.h"
#include "temperature_monitor/sensor_config.h"

#define EEPROM_I2C_ADDRESS       (0x50U)
#define EEPROM_REVISION_ADDRESS  (0U)
#define EEPROM_SERIAL_ADDRESS    (1U)
#define EEPROM_SERIAL_LENGTH     (7U)

typedef struct {
    sensor_revision_t hardware_revision;
    char serial_number[EEPROM_SERIAL_LENGTH + 1U];
} eeprom_config_t;

int eeprom_read_config(
    const i2c_t *i2c,
    eeprom_config_t *config);

int eeprom_write_config(
    const i2c_t *i2c,
    const eeprom_config_t *config);

#endif
