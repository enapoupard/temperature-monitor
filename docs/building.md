# Building and Testing

## 1. Purpose

This document describes how to configure, build, and test the PC demonstration
from the command line.

The current repository contains the build-system skeleton. Executable,
library, and test targets will be added during implementation.

## 2. Prerequisites

The project requires:

- CMake 3.16 or newer
- A C compiler supporting C11
- A C++ compiler supporting C++17
- A CMake-supported native build tool

On Linux, GCC or Clang can be used.

Check the installed tools with:

    cmake --version
    cc --version
    c++ --version

## 3. Configure

From the repository root, configure an out-of-source debug build:

    cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug

The `build` directory contains generated files and is excluded from Git.

In-source builds are intentionally rejected.

## 4. Build

Build all enabled targets with:

    cmake --build build

For parallel compilation, CMake can select an appropriate parallelism level:

    cmake --build build --parallel

## 5. Test

Run tests through CTest:

    ctest --test-dir build --output-on-failure

No tests are registered in the initial build skeleton. Tests will be added with
the domain logic.

## 6. Build Options

The project provides these CMake options:

| Option | Default | Purpose |
|---|---:|---|
| `TEMPERATURE_MONITOR_BUILD_C` | `ON` | Enable the C implementation |
| `TEMPERATURE_MONITOR_BUILD_CPP` | `ON` | Enable the C++ implementation |
| `TEMPERATURE_MONITOR_ENABLE_STRICT_WARNINGS` | `ON` | Enable strict compiler warnings |
| `BUILD_TESTING` | `ON` | Enable CTest and future test targets |

For example, configure only the C implementation:

    cmake -S . -B build-c \
        -DCMAKE_BUILD_TYPE=Debug \
        -DTEMPERATURE_MONITOR_BUILD_C=ON \
        -DTEMPERATURE_MONITOR_BUILD_CPP=OFF

Configure only the C++ implementation:

    cmake -S . -B build-cpp \
        -DCMAKE_BUILD_TYPE=Debug \
        -DTEMPERATURE_MONITOR_BUILD_C=OFF \
        -DTEMPERATURE_MONITOR_BUILD_CPP=ON

Disable tests:

    cmake -S . -B build-no-tests \
        -DBUILD_TESTING=OFF

## 7. Clean Build

Remove generated build output and configure again:

    rm -rf build
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
    cmake --build build

Only generated build directories should be removed with `rm -rf`. Verify the
current directory and command carefully before executing it.

## 8. Compiler Selection

Select GCC explicitly:

    cmake -S . -B build-gcc \
        -DCMAKE_C_COMPILER=gcc \
        -DCMAKE_CXX_COMPILER=g++

Select Clang explicitly:

    cmake -S . -B build-clang \
        -DCMAKE_C_COMPILER=clang \
        -DCMAKE_CXX_COMPILER=clang++

Compiler selection should be made when creating a new build directory. Do not
change compilers inside an already configured build directory.

## 9. Generated Compile Commands

The build configuration generates:

    build/compile_commands.json

This file can be used by compatible IDEs, language servers, and static-analysis
tools. It is generated data and is not committed to the repository.

## 10. Current Limitations

At this stage:

- No application executables are defined
- No production libraries are defined
- No unit tests are registered
- No hardware mocks are built
- No embedded toolchain is configured

A successful build currently verifies only the build-system configuration.
