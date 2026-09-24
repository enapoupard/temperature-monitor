#include "temperature_monitor/sensor_config.h"

static const sensor_config_t configurations[] = {
    { SENSOR_REV_A, 1.0 },
    { SENSOR_REV_B, 0.1 }
};

const sensor_config_t *sensor_config_get(sensor_revision_t revision)
{
    if ((revision != SENSOR_REV_A) && (revision != SENSOR_REV_B)) {
        return 0;
    }

    return &configurations[(unsigned int)revision];
}
