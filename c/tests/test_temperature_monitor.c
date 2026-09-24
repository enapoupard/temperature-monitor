#include "temperature_monitor/adc_conversion.h"
#include "temperature_monitor/sensor_config.h"
#include "temperature_monitor/temperature_classification.h"

#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

static int nearly_equal(double a, double b)
{
    return fabs(a - b) < 1.0e-12;
}

static void test_adc_examples(void)
{
    const sensor_config_t *rev_a = sensor_config_get(SENSOR_REV_A);
    const sensor_config_t *rev_b = sensor_config_get(SENSOR_REV_B);
    double temperature = 0.0;
    uint32_t digit = 0U;

    assert(rev_a != NULL);
    assert(rev_b != NULL);
    assert(sensor_config_get((sensor_revision_t)2) == NULL);

    assert(adc_to_temperature(10U, rev_a, &temperature) ==
           ADC_CONVERSION_OK);
    assert(nearly_equal(temperature, 10.0));

    assert(adc_to_temperature(100U, rev_b, &temperature) ==
           ADC_CONVERSION_OK);
    assert(nearly_equal(temperature, 10.0));

    assert(temperature_to_adc(10.0, rev_a, &digit) ==
           ADC_CONVERSION_OK);
    assert(digit == 10U);

    assert(temperature_to_adc(10.0, rev_b, &digit) ==
           ADC_CONVERSION_OK);
    assert(digit == 100U);
}

static void test_adc_boundaries(void)
{
    const sensor_config_t *rev_a = sensor_config_get(SENSOR_REV_A);
    const sensor_config_t *rev_b = sensor_config_get(SENSOR_REV_B);
    const double temperatures[] = { 4.0, 5.0, 84.0, 85.0, 105.0 };
    const uint32_t rev_a_expected[] = { 4U, 5U, 84U, 85U, 105U };
    const uint32_t rev_b_expected[] = { 40U, 50U, 840U, 850U, 1050U };
    uint32_t digit = 0U;
    double decoded = 0.0;
    size_t i;

    for (i = 0U; i < sizeof(temperatures) / sizeof(temperatures[0]); ++i) {
        assert(temperature_to_adc(temperatures[i], rev_a, &digit) ==
               ADC_CONVERSION_OK);
        assert(digit == rev_a_expected[i]);
        assert(adc_to_temperature(digit, rev_a, &decoded) ==
               ADC_CONVERSION_OK);
        assert(nearly_equal(decoded, temperatures[i]));

        assert(temperature_to_adc(temperatures[i], rev_b, &digit) ==
               ADC_CONVERSION_OK);
        assert(digit == rev_b_expected[i]);
        assert(adc_to_temperature(digit, rev_b, &decoded) ==
               ADC_CONVERSION_OK);
        assert(nearly_equal(decoded, temperatures[i]));
    }
}

static void test_adc_quantization(void)
{
    const sensor_config_t *rev_a = sensor_config_get(SENSOR_REV_A);
    const sensor_config_t *rev_b = sensor_config_get(SENSOR_REV_B);
    uint32_t digit = 0U;
    double decoded = 0.0;

    assert(temperature_to_adc(84.6, rev_a, &digit) ==
           ADC_CONVERSION_OK);
    assert(digit == 85U);
    assert(adc_to_temperature(digit, rev_a, &decoded) ==
           ADC_CONVERSION_OK);
    assert(nearly_equal(decoded, 85.0));

    assert(temperature_to_adc(84.96, rev_b, &digit) ==
           ADC_CONVERSION_OK);
    assert(digit == 850U);
    assert(adc_to_temperature(digit, rev_b, &decoded) ==
           ADC_CONVERSION_OK);
    assert(nearly_equal(decoded, 85.0));
}

static void test_adc_errors(void)
{
    const sensor_config_t *rev_a = sensor_config_get(SENSOR_REV_A);
    uint32_t digit = 0U;
    double temperature = 0.0;

    assert(adc_to_temperature(0U, rev_a, NULL) ==
           ADC_CONVERSION_INVALID_ARGUMENT);
    assert(adc_to_temperature(0U, NULL, &temperature) ==
           ADC_CONVERSION_INVALID_CONFIG);
    assert(temperature_to_adc(0.0, rev_a, NULL) ==
           ADC_CONVERSION_INVALID_ARGUMENT);
    assert(temperature_to_adc(-0.1, rev_a, &digit) ==
           ADC_CONVERSION_OUT_OF_RANGE);
    assert(temperature_to_adc(NAN, rev_a, &digit) ==
           ADC_CONVERSION_OUT_OF_RANGE);
    assert(temperature_to_adc(INFINITY, rev_a, &digit) ==
           ADC_CONVERSION_OUT_OF_RANGE);
}

static void test_classification_boundaries(void)
{
    assert(temperature_classify(4.9) == TEMPERATURE_STATUS_RED);
    assert(temperature_classify(5.0) == TEMPERATURE_STATUS_GREEN);
    assert(temperature_classify(84.9) == TEMPERATURE_STATUS_GREEN);
    assert(temperature_classify(85.0) == TEMPERATURE_STATUS_YELLOW);
    assert(temperature_classify(104.9) == TEMPERATURE_STATUS_YELLOW);
    assert(temperature_classify(105.0) == TEMPERATURE_STATUS_RED);

    assert(temperature_classify(NAN) == TEMPERATURE_STATUS_INVALID);
    assert(temperature_classify(INFINITY) == TEMPERATURE_STATUS_INVALID);
    assert(temperature_classify(-INFINITY) == TEMPERATURE_STATUS_INVALID);
}

int main(void)
{
    test_adc_examples();
    test_adc_boundaries();
    test_adc_quantization();
    test_adc_errors();
    test_classification_boundaries();

    puts("All temperature-monitor tests passed.");
    return 0;
}
