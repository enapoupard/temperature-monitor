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

message(STATUS "Configured C implementation build skeleton")
