#include "temperature_monitor/sampling_timer.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

static void count_sample(void *context)
{
    uint32_t *count = context;
    ++(*count);
}

int main(void)
{
    uint32_t count = 0U;

    assert(sampling_timer_init(
               TEMPERATURE_SAMPLE_PERIOD_US,
               count_sample,
               &count) == 0);

    sampling_timer_irq_handler();
    assert(count == 0U);

    sampling_timer_start();

    for (uint32_t i = 0U; i < 1000U; ++i) {
        sampling_timer_irq_handler();
    }

    assert(count == 1000U);

    sampling_timer_stop();
    sampling_timer_irq_handler();
    assert(count == 1000U);

    assert(sampling_timer_init(99U, count_sample, &count) == -1);
    assert(sampling_timer_init(
               TEMPERATURE_SAMPLE_PERIOD_US,
               0,
               &count) == -1);

    puts("Sampling timer tests passed.");
    return 0;
}
