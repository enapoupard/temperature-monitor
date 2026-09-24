#ifndef TEMPERATURE_MONITOR_GPIO_H
#define TEMPERATURE_MONITOR_GPIO_H

#include "temperature_monitor/temperature_classification.h"

typedef struct {
    int green;
    int yellow;
    int red;
} gpio_led_state_t;

/*
 * Platform-specific GPIO function.
 *
 * A real microcontroller implementation will use this callback to write
 * the physical GPIO registers. A unit test can provide a fake function.
 */
typedef void (*gpio_write_function_t)(
    const gpio_led_state_t *state,
    void *context);

typedef struct {
    gpio_write_function_t write;
    void *context;
} gpio_t;

int gpio_init(
    gpio_t *gpio,
    gpio_write_function_t write_function,
    void *context);

void gpio_set_temperature_status(
    const gpio_t *gpio,
    temperature_status_t status);

#endif
