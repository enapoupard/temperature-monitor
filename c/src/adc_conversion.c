#include "temperature_monitor/adc_conversion.h"

#include <limits.h>
#include <math.h>

static int config_is_valid(const sensor_config_t *config)
{
    return (config != 0) &&
           ((config->revision == SENSOR_REV_A) ||
            (config->revision == SENSOR_REV_B)) &&
           isfinite(config->degrees_c_per_digit) &&
           (config->degrees_c_per_digit > 0.0);
}

adc_conversion_status_t adc_to_temperature(
    uint32_t adc_digit,
    const sensor_config_t *config,
    double *temperature_c)
{
    if (temperature_c == 0) {
        return ADC_CONVERSION_INVALID_ARGUMENT;
    }

    if (!config_is_valid(config)) {
        return ADC_CONVERSION_INVALID_CONFIG;
    }

    *temperature_c = (double)adc_digit * config->degrees_c_per_digit;
    return ADC_CONVERSION_OK;
}

adc_conversion_status_t temperature_to_adc(
    double temperature_c,
    const sensor_config_t *config,
    uint32_t *adc_digit)
{
    double digit;

    if (adc_digit == 0) {
        return ADC_CONVERSION_INVALID_ARGUMENT;
    }

    if (!config_is_valid(config)) {
        return ADC_CONVERSION_INVALID_CONFIG;
    }

    if (!isfinite(temperature_c) || (temperature_c < 0.0)) {
        return ADC_CONVERSION_OUT_OF_RANGE;
    }

    digit = temperature_c / config->degrees_c_per_digit;

    if (digit > (double)UINT32_MAX) {
        return ADC_CONVERSION_OUT_OF_RANGE;
    }

    *adc_digit = (uint32_t)floor(digit + 0.5);
    return ADC_CONVERSION_OK;
}
