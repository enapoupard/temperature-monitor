#include "temperature_monitor/temperature_classification.h"

#include <math.h>

temperature_status_t temperature_classify(double temperature_c)
{
    if (!isfinite(temperature_c)) {
        return TEMPERATURE_STATUS_INVALID;
    }

    if ((temperature_c < 5.0) || (temperature_c >= 105.0)) {
        return TEMPERATURE_STATUS_RED;
    }

    if (temperature_c >= 85.0) {
        return TEMPERATURE_STATUS_YELLOW;
    }

    return TEMPERATURE_STATUS_GREEN;
}
