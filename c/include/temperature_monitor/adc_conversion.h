#ifndef TEMPERATURE_MONITOR_ADC_CONVERSION_H
#define TEMPERATURE_MONITOR_ADC_CONVERSION_H

#include <stdint.h>

#include "temperature_monitor/sensor_config.h"

typedef enum {
    ADC_CONVERSION_OK = 0,
    ADC_CONVERSION_INVALID_ARGUMENT,
    ADC_CONVERSION_INVALID_CONFIG,
    ADC_CONVERSION_OUT_OF_RANGE
} adc_conversion_status_t;

adc_conversion_status_t adc_to_temperature(
    uint32_t adc_digit,
    const sensor_config_t *config,
    double *temperature_c);

adc_conversion_status_t temperature_to_adc(
    double temperature_c,
    const sensor_config_t *config,
    uint32_t *adc_digit);

#endif
