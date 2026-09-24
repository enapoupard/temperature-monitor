#include "temperature_monitor/gpio.h"

#include <assert.h>
#include <stdio.h>

static void fake_gpio_write(
    const gpio_led_state_t *state,
    void *context)
{
    gpio_led_state_t *captured_state = context;

    *captured_state = *state;
}

static void assert_leds(
    const gpio_led_state_t *state,
    int green,
    int yellow,
    int red)
{
    assert(state->green == green);
    assert(state->yellow == yellow);
    assert(state->red == red);
}

int main(void)
{
    gpio_t gpio;
    gpio_led_state_t captured_state = { 0, 0, 0 };

    assert(gpio_init(&gpio, fake_gpio_write, &captured_state) == 0);
    assert(gpio_init(NULL, fake_gpio_write, &captured_state) == -1);
    assert(gpio_init(&gpio, NULL, &captured_state) == -1);

    assert(gpio_init(&gpio, fake_gpio_write, &captured_state) == 0);

    gpio_set_temperature_status(
        &gpio,
        TEMPERATURE_STATUS_GREEN);
    assert_leds(&captured_state, 1, 0, 0);

    gpio_set_temperature_status(
        &gpio,
        TEMPERATURE_STATUS_YELLOW);
    assert_leds(&captured_state, 0, 1, 0);

    gpio_set_temperature_status(
        &gpio,
        TEMPERATURE_STATUS_RED);
    assert_leds(&captured_state, 0, 0, 1);

    gpio_set_temperature_status(
        &gpio,
        TEMPERATURE_STATUS_INVALID);
    assert_leds(&captured_state, 0, 0, 1);

    gpio_set_temperature_status(
        &gpio,
        (temperature_status_t)99);
    assert_leds(&captured_state, 0, 0, 1);

    /*
     * A null GPIO object is ignored safely.
     */
    gpio_set_temperature_status(
        NULL,
        TEMPERATURE_STATUS_GREEN);

    puts("GPIO tests passed.");
    return 0;
}
