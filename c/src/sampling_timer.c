#include "temperature_monitor/sampling_timer.h"

static sampling_callback_t sample_callback;
static void *sample_context;
static int timer_started;

int sampling_timer_init(uint32_t period_us,
                        sampling_callback_t callback,
                        void *context)
{
    if ((period_us != TEMPERATURE_SAMPLE_PERIOD_US) || (callback == 0)) {
        return -1;
    }

    sample_callback = callback;
    sample_context = context;
    timer_started = 0;
    return 0;
}

void sampling_timer_start(void)
{
    timer_started = 1;
}

void sampling_timer_stop(void)
{
    timer_started = 0;
}

void sampling_timer_irq_handler(void)
{
    if ((timer_started != 0) && (sample_callback != 0)) {
        sample_callback(sample_context);
    }
}
