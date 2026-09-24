#include <stdio.h>
#include <math.h>

/**
 * @brief Simulates a first-order thermal system controlled by a PI controller.
 *
 * This function encapsulates a complete simulation loop to determine the steady-state
 * temperature of a system for a given set-point. It combines:
 * 1. A PI Controller (in standard form with anti-windup).
 * 2. A first-order ODE model of a thermal plant (the "system").
 * 3. A 4th-order Runge-Kutta (RK4) numerical solver to integrate the ODE.
 *
 * The entire simulation runs for a fixed duration to allow the system to
 * reach a stable temperature. This function is self-contained and does not
 * depend on external state.
 *
 * @param set_point_celsius The target temperature for the system in degrees Celsius.
 * @return The final temperature of the system after the simulation completes.
 */
double simulate_temperature_system(double set_point_celsius) {
    // --- 1. System and Simulation Parameters ---
    const double AMBIENT_TEMP_C = 20.0;     // Temperature of the surroundings
    const double TIME_CONSTANT_S = 120.0;   // Thermal time constant of the system in seconds
    const double SIMULATION_TIME_S = 1000.0;// Total simulation duration
    const double TIME_STEP_S = 0.1;         // Integration time step

    // --- 2. PI Controller Parameters ---
    const double KP = 10.0;                 // Proportional gain
    const double KI = 0.1;                  // Integral gain
    const double HEATER_MAX_POWER = 150.0;  // Max "power" (effective temp) the heater can apply
    const double HEATER_MIN_POWER = 0.0;

    // --- 3. State Variables ---
    double current_temp_c = AMBIENT_TEMP_C; // System starts at ambient temperature
    double pi_integrator = 0.0;             // Integral term for the PI controller
    double heater_power = 0.0;              // Current output of the PI controller

    // --- 4. Main Simulation Loop ---
    int num_steps = (int)(SIMULATION_TIME_S / TIME_STEP_S);
    for (int i = 0; i < num_steps; ++i) {
        // a. PI Controller Logic
        double error = set_point_celsius - current_temp_c;

        // Update integrator with anti-windup: only integrate if the output is not saturated
        if ((heater_power > HEATER_MIN_POWER && heater_power < HEATER_MAX_POWER) ||
            (heater_power <= HEATER_MIN_POWER && error > 0) ||
            (heater_power >= HEATER_MAX_POWER && error < 0)) {
            pi_integrator += error * TIME_STEP_S;
        }

        // Calculate controller output
        heater_power = (KP * error) + (KI * pi_integrator);

        // Clamp output (final anti-windup step)
        if (heater_power > HEATER_MAX_POWER) {
            heater_power = HEATER_MAX_POWER;
        } else if (heater_power < HEATER_MIN_POWER) {
            heater_power = HEATER_MIN_POWER;
        }

        // b. ODE Solver (RK4)
        // The ODE to solve is: dT/dt = (1/tau) * (heater_power - (T - T_ambient))
        // This models the heater adding energy and the system losing energy to the ambient.
        double k1, k2, k3, k4;
        double temp_k2, temp_k3, temp_k4;

        // k1
        k1 = (1.0 / TIME_CONSTANT_S) * (heater_power - (current_temp_c - AMBIENT_TEMP_C));

        // k2
        temp_k2 = current_temp_c + (k1 * TIME_STEP_S / 2.0);
        k2 = (1.0 / TIME_CONSTANT_S) * (heater_power - (temp_k2 - AMBIENT_TEMP_C));

        // k3
        temp_k3 = current_temp_c + (k2 * TIME_STEP_S / 2.0);
        k3 = (1.0 / TIME_CONSTANT_S) * (heater_power - (temp_k3 - AMBIENT_TEMP_C));

        // k4
        temp_k4 = current_temp_c + (k3 * TIME_STEP_S);
        k4 = (1.0 / TIME_CONSTANT_S) * (heater_power - (temp_k4 - AMBIENT_TEMP_C));

        // Update the temperature state
        current_temp_c += (TIME_STEP_S / 6.0) * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
    }

    // --- 5. Return Final State ---
    return current_temp_c;
}

/**
 * @brief Main entry point to demonstrate the temperature simulation.
 *
 * This program calls the simulation function with several different set-points
 * to generate temperatures across the required operational ranges.
 */
int main(void) {
    printf("--- Temperature System Simulation ---\n");

    // Define set-points to target the required temperature ranges
    // (<5, <85, >=85, >=105)
    double set_points[] = {
        0.0,    // Target: Below 5°C
        80.0,   // Target: Below 85°C
        95.0,   // Target: At or above 85°C
        110.0   // Target: At or above 105°C
    };
    int num_set_points = sizeof(set_points) / sizeof(set_points[0]);

    for (int i = 0; i < num_set_points; ++i) {
        double set_point = set_points[i];
        double final_temp = simulate_temperature_system(set_point);
        printf("Set-point: %6.2f°C -> Final Simulated Temp: %6.2f°C\n", set_point, final_temp);
    }

    printf("--- Simulation Complete ---\n");

    return 0;
}
