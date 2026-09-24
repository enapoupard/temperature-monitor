#ifndef TEMPERATURE_MONITOR_SENSOR_CONFIG_H
#define TEMPERATURE_MONITOR_SENSOR_CONFIG_H

typedef enum {
    SENSOR_REV_A = 0,
    SENSOR_REV_B = 1
} sensor_revision_t;

typedef struct {
    sensor_revision_t revision;
    double degrees_c_per_digit;
} sensor_config_t;

const sensor_config_t *sensor_config_get(sensor_revision_t revision);

#endif
