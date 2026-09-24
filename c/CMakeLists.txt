# C implementation build configuration.

add_library(temperature_monitor_c_build_config INTERFACE)

target_compile_features(
    temperature_monitor_c_build_config
    INTERFACE
        c_std_11
)

target_link_libraries(
    temperature_monitor_c_build_config
    INTERFACE
        temperature_monitor_c_warnings
)

# --- Add the simulation executable ---
add_executable(
    c_temperature_simulation
    src/temperature_simulation.c
)

# Link the executable to the common build configuration and math library
target_link_libraries(
    c_temperature_simulation
    PRIVATE
        temperature_monitor_c_build_config
        m # Required for math functions like sqrt, pow, etc. on some platforms
)

message(STATUS "Configured C implementation with simulation executable")
