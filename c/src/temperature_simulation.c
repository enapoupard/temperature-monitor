#include <math.h>
#include <stdio.h>

double simulate_temperature_system(double setpoint_c)
{
    const double ambient_c = 0.0;
    const double tau_s = 20.0;
    const double kp = 2.0;
    const double ki = 0.5;
    const double dt_s = 0.01;
    const double simulation_time_s = 200.0;
    const double output_min = -120.0;
    const double output_max = 120.0;
    double temperature_c = ambient_c;
    double previous_error = setpoint_c - temperature_c;
    double output = 0.0;
    int steps = (int)(simulation_time_s / dt_s);

    if (!isfinite(setpoint_c) || setpoint_c < 0.0 || setpoint_c > 120.0) {
        return NAN;
    }

    for (int i = 0; i < steps; ++i) {
        const double error = setpoint_c - temperature_c;
        const double candidate =
            output + kp * (error - previous_error) + ki * dt_s * error;
        double applied = candidate;

        if (applied > output_max) {
            applied = output_max;
        } else if (applied < output_min) {
            applied = output_min;
        }

        /*
         * First-order thermal plant:
         * dT/dt = (ambient + actuator - T) / tau
         *
         * Fixed-step RK4 with constant actuator during this sample.
         */
        const double k1 =
            (ambient_c + applied - temperature_c) / tau_s;
        const double k2 =
            (ambient_c + applied -
             (temperature_c + 0.5 * dt_s * k1)) / tau_s;
        const double k3 =
            (ambient_c + applied -
             (temperature_c + 0.5 * dt_s * k2)) / tau_s;
        const double k4 =
            (ambient_c + applied -
             (temperature_c + dt_s * k3)) / tau_s;

        temperature_c +=
            (dt_s / 6.0) * (k1 + 2.0 * k2 + 2.0 * k3 + k4);

        output = applied;
        previous_error = error;
    }

    return temperature_c;
}

int main(void)
{
    const double setpoints_c[] = { 0.0, 4.0, 80.0, 85.0, 105.0, 120.0 };
    const int count =
        (int)(sizeof(setpoints_c) / sizeof(setpoints_c[0]));

    for (int i = 0; i < count; ++i) {
        const double temperature_c =
            simulate_temperature_system(setpoints_c[i]);

        printf("Setpoint: %6.1f C -> Temperature: %8.3f C\n",
               setpoints_c[i],
               temperature_c);
    }

    return 0;
}
