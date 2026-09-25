#include "temperature_monitor/sampling_timer.hpp"

#include <cassert>
#include <chrono>
#include <cstdint>
#include <functional>
#include <iostream>
#include <stdexcept>

namespace tempmon = temperature_monitor;

namespace {

void test_timer_constants()
{
    static_assert(
        tempmon::SamplingTimer::sample_period ==
        std::chrono::microseconds{100});

    static_assert(
        tempmon::SamplingTimer::sample_rate_hz == 10'000U);
}

void test_stopped_timer_does_not_dispatch()
{
    std::uint32_t callback_count{0U};

    tempmon::SamplingTimer timer{
        [&callback_count]() {
            ++callback_count;
        }};

    assert(!timer.is_running());

    timer.irq_handler();

    assert(callback_count == 0U);
}

void test_started_timer_dispatches_every_interrupt()
{
    std::uint32_t callback_count{0U};

    tempmon::SamplingTimer timer{
        [&callback_count]() {
            ++callback_count;
        }};

    timer.start();
    assert(timer.is_running());

    for (std::uint32_t index{0U}; index < 1'000U; ++index) {
        timer.irq_handler();
    }

    assert(callback_count == 1'000U);
}

void test_stopped_timer_stops_dispatching()
{
    std::uint32_t callback_count{0U};

    tempmon::SamplingTimer timer{
        [&callback_count]() {
            ++callback_count;
        }};

    timer.start();
    timer.irq_handler();

    assert(callback_count == 1U);

    timer.stop();
    assert(!timer.is_running());

    timer.irq_handler();

    assert(callback_count == 1U);
}

void test_timer_can_restart()
{
    std::uint32_t callback_count{0U};

    tempmon::SamplingTimer timer{
        [&callback_count]() {
            ++callback_count;
        }};

    timer.start();
    timer.irq_handler();
    timer.stop();
    timer.irq_handler();
    timer.start();
    timer.irq_handler();

    assert(callback_count == 2U);
}

void test_empty_callback_is_rejected()
{
    bool exception_was_thrown{false};

    try {
        tempmon::SamplingTimer timer{
            std::function<void()>{}};
        (void)timer;
    } catch (const std::invalid_argument&) {
        exception_was_thrown = true;
    }

    assert(exception_was_thrown);
}

} // namespace

int main()
{
    test_timer_constants();
    test_stopped_timer_does_not_dispatch();
    test_started_timer_dispatches_every_interrupt();
    test_stopped_timer_stops_dispatching();
    test_timer_can_restart();
    test_empty_callback_is_rejected();

    std::cout << "C++ sampling timer tests passed.\n";
    return 0;
}
