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
