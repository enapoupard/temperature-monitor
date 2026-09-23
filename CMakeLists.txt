cmake_minimum_required(VERSION 3.16)

project(
    temperature_monitor
    VERSION 0.1.0
    DESCRIPTION "Bare-metal temperature monitoring software demonstration"
    LANGUAGES C CXX
)

# Do not permit in-source builds because generated files must not be mixed
# with source-controlled project files.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(
        FATAL_ERROR
        "In-source builds are not supported. "
        "Use: cmake -S . -B build"
    )
endif()

# Export compiler commands for static-analysis and development tools.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Language standards used by all project targets.
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(
    TEMPERATURE_MONITOR_BUILD_C
    "Build the C implementation"
    ON
)

option(
    TEMPERATURE_MONITOR_BUILD_CPP
    "Build the C++ implementation"
    ON
)

option(
    TEMPERATURE_MONITOR_ENABLE_STRICT_WARNINGS
    "Enable strict compiler warnings for project targets"
    ON
)

include(CTest)

# Common warning configuration is represented by interface libraries.
# Application and test targets will link to the appropriate interface later.
add_library(temperature_monitor_c_warnings INTERFACE)
add_library(temperature_monitor_cpp_warnings INTERFACE)

if(TEMPERATURE_MONITOR_ENABLE_STRICT_WARNINGS)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(
            temperature_monitor_c_warnings
            INTERFACE
                -Wall
                -Wextra
                -Wpedantic
                -Werror
        )
    elseif(MSVC)
        target_compile_options(
            temperature_monitor_c_warnings
            INTERFACE
                /W4
                /WX
        )
    endif()

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(
            temperature_monitor_cpp_warnings
            INTERFACE
                -Wall
                -Wextra
                -Wpedantic
                -Werror
        )
    elseif(MSVC)
        target_compile_options(
            temperature_monitor_cpp_warnings
            INTERFACE
                /W4
                /WX
        )
    endif()
endif()

if(TEMPERATURE_MONITOR_BUILD_C)
    add_subdirectory(c)
endif()

if(TEMPERATURE_MONITOR_BUILD_CPP)
    add_subdirectory(cpp)
endif()

message(STATUS "Temperature Monitor version: ${PROJECT_VERSION}")
message(STATUS "Build C implementation: ${TEMPERATURE_MONITOR_BUILD_C}")
message(STATUS "Build C++ implementation: ${TEMPERATURE_MONITOR_BUILD_CPP}")
message(STATUS "Build tests: ${BUILD_TESTING}")
message(
    STATUS
    "Strict warnings: ${TEMPERATURE_MONITOR_ENABLE_STRICT_WARNINGS}"
)
