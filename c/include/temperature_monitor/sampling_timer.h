#ifndef TEMPERATURE_MONITOR_SAMPLING_TIMER_H
#define TEMPERATURE_MONITOR_SAMPLING_TIMER_H

#include <stdint.h>

#define TEMPERATURE_SAMPLE_PERIOD_US  (100U)
#define TEMPERATURE_SAMPLE_RATE_HZ    (10000U)

typedef void (*sampling_callback_t)(void *context);

int sampling_timer_init(uint32_t period_us,
                        sampling_callback_t callback,
                        void *context);

void sampling_timer_start(void);
void sampling_timer_stop(void);

/* Called by the target-specific timer interrupt handler. */
void sampling_timer_irq_handler(void);

#endif
