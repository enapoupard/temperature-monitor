#include "temperature_monitor/gpio.hpp"

#include <cassert>
#include <functional>
#include <iostream>
#include <stdexcept>

namespace tempmon = temperature_monitor;

namespace {

void assert_led_state(
    const tempmon::LedState& state,
    bool green,
    bool yellow,
    bool red)
{
    assert(state.green == green);
    assert(state.yellow == yellow);
    assert(state.red == red);
}

void test_green_status()
{
    tempmon::LedState captured_state{};

    tempmon::Gpio gpio{
        [&captured_state](const tempmon::LedState& state) {
            captured_state = state;
        }};

    gpio.set_temperature_status(
        tempmon::TemperatureStatus::green);

    assert_led_state(captured_state, true, false, false);
}

void test_yellow_status()
{
    tempmon::LedState captured_state{};

    tempmon::Gpio gpio{
        [&captured_state](const tempmon::LedState& state) {
            captured_state = state;
        }};

    gpio.set_temperature_status(
        tempmon::TemperatureStatus::yellow);

    assert_led_state(captured_state, false, true, false);
}

void test_red_status()
{
    tempmon::LedState captured_state{};

    tempmon::Gpio gpio{
        [&captured_state](const tempmon::LedState& state) {
            captured_state = state;
        }};

    gpio.set_temperature_status(
        tempmon::TemperatureStatus::red);

    assert_led_state(captured_state, false, false, true);
}

void test_invalid_status_is_fail_safe_red()
{
    tempmon::LedState captured_state{};

    tempmon::Gpio gpio{
        [&captured_state](const tempmon::LedState& state) {
            captured_state = state;
        }};

    gpio.set_temperature_status(
        tempmon::TemperatureStatus::invalid);

    assert_led_state(captured_state, false, false, true);
}

void test_unknown_status_is_fail_safe_red()
{
    tempmon::LedState captured_state{};

    tempmon::Gpio gpio{
        [&captured_state](const tempmon::LedState& state) {
            captured_state = state;
        }};

    const auto unknown_status =
        static_cast<tempmon::TemperatureStatus>(99);

    gpio.set_temperature_status(unknown_status);

    assert_led_state(captured_state, false, false, true);
}

void test_write_function_is_called_once()
{
    unsigned int write_count{0U};

    tempmon::Gpio gpio{
        [&write_count](const tempmon::LedState&) {
            ++write_count;
        }};

    gpio.set_temperature_status(
        tempmon::TemperatureStatus::green);

    assert(write_count == 1U);
}

void test_empty_write_function_is_rejected()
{
    bool exception_was_thrown{false};

    try {
        tempmon::Gpio gpio{
            std::function<void(const tempmon::LedState&)>{}};
        (void)gpio;
    } catch (const std::invalid_argument&) {
        exception_was_thrown = true;
    }

    assert(exception_was_thrown);
}

} // namespace

int main()
{
    test_green_status();
    test_yellow_status();
    test_red_status();
    test_invalid_status_is_fail_safe_red();
    test_unknown_status_is_fail_safe_red();
    test_write_function_is_called_once();
    test_empty_write_function_is_rejected();

    std::cout << "C++ GPIO tests passed.\n";
    return 0;
}
