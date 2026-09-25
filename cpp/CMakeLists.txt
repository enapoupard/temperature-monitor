# C++ implementation build configuration.

add_library(temperature_monitor_cpp_build_config INTERFACE)

target_compile_features(
    temperature_monitor_cpp_build_config
    INTERFACE
        cxx_std_17
)

target_link_libraries(
    temperature_monitor_cpp_build_config
    INTERFACE
        temperature_monitor_cpp_warnings
)

message(STATUS "Configured C++ implementation build skeleton")

add_executable(
    cpp_adc_conversion_tests
    tests/test_adc_conversion.cpp
    src/adc_conversion.cpp
)

target_include_directories(
    cpp_adc_conversion_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
    cpp_adc_conversion_tests
    PRIVATE
        temperature_monitor_cpp_build_config
)

add_test(
    NAME cpp_adc_conversion_tests
    COMMAND cpp_adc_conversion_tests
)

add_executable(
    cpp_temperature_classification_tests
    tests/test_temperature_classification.cpp
    src/temperature_classification.cpp
)

target_include_directories(
    cpp_temperature_classification_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
    cpp_temperature_classification_tests
    PRIVATE
        temperature_monitor_cpp_build_config
)

add_test(
    NAME cpp_temperature_classification_tests
    COMMAND cpp_temperature_classification_tests
)

add_executable(
    cpp_sampling_timer_tests
    tests/test_sampling_timer.cpp
    src/sampling_timer.cpp
)

target_include_directories(
    cpp_sampling_timer_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
    cpp_sampling_timer_tests
    PRIVATE
        temperature_monitor_cpp_build_config
)

add_test(
    NAME cpp_sampling_timer_tests
    COMMAND cpp_sampling_timer_tests
)

add_executable(
    cpp_gpio_tests
    tests/test_gpio.cpp
    src/gpio.cpp
    src/temperature_classification.cpp
)

target_include_directories(
    cpp_gpio_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
    cpp_gpio_tests
    PRIVATE
        temperature_monitor_cpp_build_config
)

add_test(
    NAME cpp_gpio_tests
    COMMAND cpp_gpio_tests
)

add_executable(
    cpp_eeprom_tests
    tests/test_eeprom.cpp
    src/i2c.cpp
    src/eeprom.cpp
)

target_include_directories(
    cpp_eeprom_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
    cpp_eeprom_tests
    PRIVATE
        temperature_monitor_cpp_build_config
)

add_test(
    NAME cpp_eeprom_tests
    COMMAND cpp_eeprom_tests
)

add_executable(
    cpp_temperature_simulation
    src/temperature_simulation.cpp
    src/adc_conversion.cpp
    src/temperature_classification.cpp
    src/sampling_timer.cpp
    src/gpio.cpp
    src/i2c.cpp
    src/eeprom.cpp
)

target_include_directories(
    cpp_temperature_simulation
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
    cpp_temperature_simulation
    PRIVATE
        temperature_monitor_cpp_build_config
)
