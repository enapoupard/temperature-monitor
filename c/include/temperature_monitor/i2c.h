#ifndef TEMPERATURE_MONITOR_I2C_H
#define TEMPERATURE_MONITOR_I2C_H

#include <stddef.h>
#include <stdint.h>

typedef int (*i2c_read_function_t)(
    uint8_t device_address,
    uint16_t memory_address,
    uint8_t *data,
    size_t length,
    void *context);

typedef int (*i2c_write_function_t)(
    uint8_t device_address,
    uint16_t memory_address,
    const uint8_t *data,
    size_t length,
    void *context);

typedef struct {
    i2c_read_function_t read;
    i2c_write_function_t write;
    void *context;
} i2c_t;

int i2c_init(
    i2c_t *i2c,
    i2c_read_function_t read_function,
    i2c_write_function_t write_function,
    void *context);

int i2c_read(
    const i2c_t *i2c,
    uint8_t device_address,
    uint16_t memory_address,
    uint8_t *data,
    size_t length);

int i2c_write(
    const i2c_t *i2c,
    uint8_t device_address,
    uint16_t memory_address,
    const uint8_t *data,
    size_t length);

#endif
