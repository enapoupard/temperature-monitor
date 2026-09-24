#include "temperature_monitor/i2c.h"

#include <stddef.h>

int i2c_init(
    i2c_t *i2c,
    i2c_read_function_t read_function,
    i2c_write_function_t write_function,
    void *context)
{
    if ((i2c == NULL) ||
        (read_function == NULL) ||
        (write_function == NULL)) {
        return -1;
    }

    i2c->read = read_function;
    i2c->write = write_function;
    i2c->context = context;

    return 0;
}

int i2c_read(
    const i2c_t *i2c,
    uint8_t device_address,
    uint16_t memory_address,
    uint8_t *data,
    size_t length)
{
    if ((i2c == NULL) ||
        (i2c->read == NULL) ||
        (data == NULL) ||
        (length == 0U)) {
        return -1;
    }

    return i2c->read(
        device_address,
        memory_address,
        data,
        length,
        i2c->context);
}

int i2c_write(
    const i2c_t *i2c,
    uint8_t device_address,
    uint16_t memory_address,
    const uint8_t *data,
    size_t length)
{
    if ((i2c == NULL) ||
        (i2c->write == NULL) ||
        (data == NULL) ||
        (length == 0U)) {
        return -1;
    }

    return i2c->write(
        device_address,
        memory_address,
        data,
        length,
        i2c->context);
}
