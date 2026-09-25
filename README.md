# Temperature Monitoring and Visualization Device

## Overview

This project implements and demonstrates software for a timer-driven
temperature-monitoring device.

A temperature sensor is represented by ADC digits, and the resulting
temperature condition is displayed using green, yellow, and red LEDs. Hardware
access is mocked so both implementations can run and be tested on a PC.

The repository contains:

- A procedural C implementation
- An object-oriented C++17 implementation
- Unit tests for both implementations
- Interactive C and C++ simulation executables
- Architecture, requirements, assumptions, and design documentation

## Temperature Conditions

| Temperature range | Condition | Active LED |
|---|---|---|
| Below $5$ degrees Celsius | Critical | Red |
| At least $5$ and below $85$ degrees Celsius | Normal | Green |
| At least $85$ and below $105$ degrees Celsius | Warning | Yellow |
| At least $105$ degrees Celsius | Critical | Red |
| Invalid data | Invalid/fail-safe | Red |

Exactly one condition LED is active after a valid sample has been classified.

## Supported Hardware Revisions

| Revision | EEPROM value | Resolution | Example at $10$ degrees Celsius |
|---|---:|---:|---:|
| Rev-A | 0 | $1.0$ degree Celsius per digit | 10 |
| Rev-B | 1 | $0.1$ degree Celsius per digit | 100 |

Only the revision selected from EEPROM is used during one execution.

## Implemented Data Flow

```text
Interactive setpoint
  -> PI-controlled thermal ODE with RK4 integration
  -> revision-dependent ADC digit
  -> 100 us sampling-timer dispatch
  -> ADC-to-temperature conversion
  -> temperature classification
  -> mocked GPIO
  -> traffic-light output
```

During initialization:

```text
Revision selection
  -> mocked I2C
  -> mocked EEPROM
  -> revision and serial-number readback
  -> sensor configuration
```

The thermal simulation is demonstration support and is not part of the core
temperature-monitoring requirements.

## Components

Both implementations provide corresponding behavior for:

- Rev-A and Rev-B ADC conversion
- Temperature classification
- Sampling-timer dispatch
- GPIO traffic-light control
- I2C transport
- EEPROM configuration
- Integrated PC simulation

The C implementation uses explicit module interfaces, state objects, and
function pointers where required.

The C++ implementation uses small classes, strongly typed enumerations,
callbacks, constructor-based dependency injection, and `std::optional` for
operations that may fail.

## Build

### Prerequisites

- CMake 3.16 or newer
- C11 compiler
- C++17 compiler
- CMake-supported native build tool

### Configure and build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
```

Strict compiler warnings are enabled by default.

## Test

```bash
ctest --test-dir build --output-on-failure
```

The test suite covers:

- Rev-A and Rev-B conversion
- ADC quantization and invalid values
- Temperature boundaries
- Sampling-timer behavior
- GPIO LED-state mapping
- Invalid and fail-safe states
- I2C validation
- EEPROM read, write, and configuration validation

## Run

Run the C simulation:

```bash
./build/c/c_temperature_simulation
```

Run the C++ simulation:

```bash
./build/cpp/cpp_temperature_simulation
```

Each executable requests:

1. Hardware revision A or B
2. Temperature setpoints from $0$ to $120$ degrees Celsius

Enter `q` to stop.

Example C++ output:

```text
Temperature Monitor
-------------------
Select hardware revision (A/B): B

Serial number:      ABC1234
Hardware revision:  Rev-B
Sensor resolution:  0.1 C/digit
Sampling period:    100 us
Valid setpoints:    0 to 120 C

Enter setpoint in C, or q to quit: 84.5

Setpoint:           84.5 C
Simulated plant:    84.500 C
ADC digit:          845
Measured:           84.5 C
Condition:          NORMAL

Traffic light
  RED    [OFF]
  YELLOW [OFF]
  GREEN  [ON ]
```

## Build Options

| Option | Default | Purpose |
|---|---:|---|
| `TEMPERATURE_MONITOR_BUILD_C` | `ON` | Build the C implementation |
| `TEMPERATURE_MONITOR_BUILD_CPP` | `ON` | Build the C++ implementation |
| `TEMPERATURE_MONITOR_ENABLE_STRICT_WARNINGS` | `ON` | Enable strict warnings |
| `BUILD_TESTING` | `ON` | Build and register tests |

For example, build only C++:

```bash
cmake -S . -B build-cpp \
    -DCMAKE_BUILD_TYPE=Debug \
    -DTEMPERATURE_MONITOR_BUILD_C=OFF \
    -DTEMPERATURE_MONITOR_BUILD_CPP=ON
cmake --build build-cpp --parallel
ctest --test-dir build-cpp --output-on-failure
```

## Repository Structure

```text
temperature-monitor/
├── CMakeLists.txt
├── README.md
├── LICENSE
├── docs/
│   ├── architecture.md
│   ├── architecture.svg
│   ├── assumptions.md
│   ├── building.md
│   ├── error-handling.md
│   ├── requirements.md
│   └── simulation-design.md
├── c/
│   ├── CMakeLists.txt
│   ├── include/
│   ├── src/
│   └── tests/
└── cpp/
    ├── CMakeLists.txt
    ├── include/
    ├── src/
    └── tests/
```

## Timing Model

The intended embedded sampling period is $100$ microseconds, corresponding to
$10$ kHz.

The PC demonstrations invoke timer dispatch synchronously and deterministically
after each accepted setpoint. They validate functional behavior, not hard
real-time scheduling, interrupt latency, jitter, ADC electrical behavior, or
GPIO electrical behavior.

## Error Handling

Invalid configuration, conversion failures, and invalid temperature data are
rejected explicitly. When reliable classification is unavailable, the logical
fail-safe LED state is:

- Green off
- Yellow off
- Red on

The LED state alone does not distinguish a system fault from a critical
temperature.

## Documentation

Detailed documentation is available in the `docs` directory:

- `architecture.md` and `architecture.svg`: software architecture
- `requirements.md`: requirements and verification overview
- `assumptions.md`: design interpretations and open questions
- `building.md`: detailed build and execution instructions
- `error-handling.md`: failure-handling policy
- `simulation-design.md`: PI, thermal ODE, RK4, and integration flow

## Project Status

The C and C++ implementations, tests, mocked interfaces, and integrated PC
demonstrations are complete for the current project scope. Target-specific
bare-metal peripheral drivers and real-time verification remain outside the PC
demonstration scope.

## License

This project is licensed under the MIT License. See `LICENSE` for details.
