# Building, Testing, and Running

## 1. Prerequisites

The project requires:

- CMake 3.16 or newer
- A C11 compiler
- A C++17 compiler
- A CMake-supported native build tool

No physical hardware, emulator, GUI library, or external runtime dependency is
required.

## 2. Configure

From the repository root:

    cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug

The generated `build` directory is excluded from Git. In-source builds are
rejected.

## 3. Build

    cmake --build build --parallel

## 4. Test

    ctest --test-dir build --output-on-failure

The test suite covers both implementations, including:

- ADC conversion
- Temperature classification
- Sampling-timer dispatch
- GPIO LED mapping
- I2C and EEPROM behavior

## 5. Run the Demonstrations

Run the C implementation:

    ./build/c/c_temperature_simulation

Run the C++ implementation:

    ./build/cpp/cpp_temperature_simulation

Each executable asks for:

1. Hardware revision A or B
2. A temperature setpoint from 0 to 120 degrees Celsius

Enter `q` to stop.

The selected revision is stored in mocked EEPROM through the I2C abstraction.
Each accepted setpoint is processed through the simulation, ADC conversion,
sampling timer, classification, and GPIO traffic-light output.

## 6. Build Options

| Option | Default | Purpose |
|---|---:|---|
| `TEMPERATURE_MONITOR_BUILD_C` | `ON` | Build the C implementation |
| `TEMPERATURE_MONITOR_BUILD_CPP` | `ON` | Build the C++ implementation |
| `TEMPERATURE_MONITOR_ENABLE_STRICT_WARNINGS` | `ON` | Enable strict warnings |
| `BUILD_TESTING` | `ON` | Build and register tests |

Build only C:

    cmake -S . -B build-c \
        -DCMAKE_BUILD_TYPE=Debug \
        -DTEMPERATURE_MONITOR_BUILD_C=ON \
        -DTEMPERATURE_MONITOR_BUILD_CPP=OFF

Build only C++:

    cmake -S . -B build-cpp \
        -DCMAKE_BUILD_TYPE=Debug \
        -DTEMPERATURE_MONITOR_BUILD_C=OFF \
        -DTEMPERATURE_MONITOR_BUILD_CPP=ON

Disable tests:

    cmake -S . -B build-no-tests \
        -DBUILD_TESTING=OFF

## 7. Clean Build

    rm -rf build
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
    cmake --build build --parallel

## 8. Limitations

The PC demonstration uses synchronous mocked timer dispatch. It does not verify
real-time scheduling, interrupt latency, sampling jitter, or electrical
hardware behavior.
