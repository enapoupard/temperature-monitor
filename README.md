# Temperature Monitoring and Visualization Device

## Overview

This project demonstrates the architecture and implementation of software for
a bare-metal embedded temperature-monitoring device.

The device samples a temperature sensor through an ADC and visualizes the
current temperature condition using green, yellow, and red LEDs connected
through GPIOs.

For demonstration and evaluation, the hardware interfaces are mocked so that
the software can run on a PC. The project contains two implementations:

- A procedural implementation written in C
- An object-oriented implementation written in C++

## Functional Requirements

The application shall:

- Sample the temperature sensor every 100 microseconds
- Minimize sampling jitter through a timer-driven embedded design
- Read the temperature sensor through an ADC abstraction
- Control three LEDs through a GPIO abstraction
- Read the hardware revision and serial number from an EEPROM abstraction
- Support Rev-A and Rev-B temperature sensors
- Ensure that only the LED associated with the current condition is active

## Temperature Conditions

The temperature conditions are interpreted as follows:

| Temperature range | Condition | Active LED |
|---|---|---|
| Below 5 degrees Celsius | Critical | Red |
| From 5 degrees Celsius up to, but not including, 85 degrees Celsius | Normal | Green |
| From 85 degrees Celsius up to, but not including, 105 degrees Celsius | Warning | Yellow |
| 105 degrees Celsius or above | Critical | Red |

The critical condition takes priority over the normal condition for
temperatures below 5 degrees Celsius.

## Supported Hardware Revisions

### Rev-A

- EEPROM hardware revision value: 0
- Sensor resolution: 1 degree Celsius per raw digit
- Example: a raw value of 10 represents 10 degrees Celsius

### Rev-B

- EEPROM hardware revision value: 1
- Sensor resolution: 0.1 degrees Celsius per raw digit
- Example: a raw value of 100 represents 10 degrees Celsius

Only one sensor type is operational at a time. The configured hardware revision
determines which sensor conversion is used.

## Internal Temperature Representation

The application uses tenths of a degree Celsius as its normalized internal
temperature unit.

Examples:

- Rev-A raw value 10 is normalized to 100
- Rev-B raw value 100 is normalized to 100
- Normalized value 100 represents 10.0 degrees Celsius

This approach allows both hardware revisions to use the same classification
logic without requiring floating-point arithmetic.

## EEPROM Configuration

The mocked EEPROM provides:

- Hardware revision
  - 0 for Rev-A
  - 1 for Rev-B
- Hardware serial number
  - Example placeholder: ABC1234

## Implementations

### C implementation

The C version uses:

- Explicit module interfaces
- Hardware-abstraction functions
- Mocked PC hardware implementations
- Testable temperature conversion and classification functions

### C++ implementation

The C++ version applies object-oriented design principles, including:

- Encapsulation
- Interface-based hardware abstractions
- Dependency injection
- Polymorphic sensor implementations
- Strongly typed states where appropriate

Both implementations are intended to provide equivalent functional behavior.

## Architecture

The planned software layers are:

1. PC demonstration application
2. Temperature-monitoring application logic
3. Sensor conversion and temperature classification
4. ADC, GPIO, EEPROM, and timer abstractions
5. PC mocks or future target-specific drivers

Architecture diagrams and detailed design documentation will be stored in the
`docs` directory.

## Timing Model

The required sampling interval is 100 microseconds, corresponding to a sampling
frequency of 10 kHz.

On an embedded target, a hardware timer would trigger ADC sampling to minimize
jitter. The interrupt service routines will be defined in the source code, but
they do not need to be invoked asynchronously by the PC demonstration.

The PC implementation demonstrates functional behavior and software
architecture. It does not guarantee hard real-time timing or prove embedded
target jitter performance.

## Planned Repository Structure

    temperature-monitor/
    ├── README.md
    ├── CMakeLists.txt
    ├── docs/
    ├── c/
    │   ├── include/
    │   ├── src/
    │   └── tests/
    ├── cpp/
    │   ├── include/
    │   ├── src/
    │   └── tests/
    ├── mocks/
    │   ├── include/
    │   └── src/
    ├── test_data/
    └── scripts/

The exact structure may evolve as the architecture is refined.

## Build Status

The build system has not yet been added.

CMake is planned as the build system for the C and C++ implementations.

## Testing Strategy

Testing will cover:

- Rev-A sensor conversion
- Rev-B sensor conversion
- Temperature classification boundaries
- LED-state mapping
- Invalid hardware revisions
- EEPROM and ADC failure handling
- Application initialization
- Temperature-state transitions
- Equivalent behavior of the C and C++ implementations

Important boundary values include:

- Below and exactly 5 degrees Celsius
- Below and exactly 85 degrees Celsius
- Below and exactly 105 degrees Celsius

## Assumptions

The initial design uses the following assumptions:

- Only one LED is active at a time
- Critical temperature has priority over normal temperature
- Temperature thresholds are fixed
- EEPROM configuration is read during initialization
- Invalid configuration or sensor data results in a safe error state
- The red LED is used to indicate a critical or error state
- Hardware interfaces are mocked for the PC demonstration
- No temperature filtering is required unless specified later

These assumptions will be reviewed and documented in more detail during the
architecture phase.

## Current Project Status

Initial repository setup and requirements documentation are in progress.

## License

This project is licensed under the MIT License. See `LICENSE` for details.
