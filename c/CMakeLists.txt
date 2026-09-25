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

target_sources(
    c_temperature_simulation
    PRIVATE
        src/sensor_config.c
        src/adc_conversion.c
)

target_include_directories(
    c_temperature_simulation
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_sources(
    c_temperature_simulation
    PRIVATE
        src/temperature_classification.c
)

add_executable(
    c_temperature_monitor_tests
    tests/test_temperature_monitor.c
    src/sensor_config.c
    src/adc_conversion.c
    src/temperature_classification.c
)

target_include_directories(
    c_temperature_monitor_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
    c_temperature_monitor_tests
    PRIVATE
        temperature_monitor_c_build_config
        m
)

add_test(
    NAME c_temperature_monitor_tests
    COMMAND c_temperature_monitor_tests
)

add_executable(
    c_sampling_timer_tests
    tests/test_sampling_timer.c
    src/sampling_timer.c
)

target_include_directories(
    c_sampling_timer_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
    c_sampling_timer_tests
    PRIVATE
        temperature_monitor_c_build_config
)

add_test(
    NAME c_sampling_timer_tests
    COMMAND c_sampling_timer_tests
)

add_executable(
    c_gpio_tests
    tests/test_gpio.c
    src/gpio.c
)

target_include_directories(
    c_gpio_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
    c_gpio_tests
    PRIVATE
        temperature_monitor_c_build_config
)

add_test(
    NAME c_gpio_tests
    COMMAND c_gpio_tests
)

add_executable(
    c_eeprom_tests
    tests/test_eeprom.c
    src/i2c.c
    src/eeprom.c
)

target_include_directories(
    c_eeprom_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
    c_eeprom_tests
    PRIVATE
        temperature_monitor_c_build_config
)

add_test(
    NAME c_eeprom_tests
    COMMAND c_eeprom_tests
)

target_sources(
    c_temperature_simulation
    PRIVATE
        src/sampling_timer.c
        src/gpio.c
        src/i2c.c
        src/eeprom.c
)
