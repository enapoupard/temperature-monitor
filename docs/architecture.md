# Software Architecture

## 1. Purpose

This document describes the initial software architecture for the temperature
monitoring and visualization device.

The architecture supports:

- A bare-metal, timer-driven embedded implementation
- A PC demonstration using mocked hardware
- Separate C and C++ implementations
- Rev-A and Rev-B temperature sensors
- Unit testing of hardware-independent behavior
- Low-jitter temperature acquisition on a future embedded target

The architecture diagram is available in `architecture.svg`.

## 2. Architectural Goals

The principal architectural goals are:

1. Separate domain logic from hardware access.
2. Keep temperature conversion and classification independently testable.
3. Support both sensor revisions without duplicating classification logic.
4. Minimize work performed in interrupt context.
5. Provide deterministic and explicit failure handling.
6. Avoid requiring dynamic memory allocation.
7. Give the C and C++ implementations equivalent externally observable
   behavior.
8. Allow PC mocks to be replaced by target-specific drivers.

## 3. System Context

The software interacts with four hardware capabilities:

- ADC for temperature sensor acquisition
- GPIO for green, yellow, and red LED control
- EEPROM for hardware revision and serial number
- Hardware timer for periodic acquisition

The PC demonstration replaces these capabilities with software mocks.

## 4. Layered Architecture

The architecture is divided into the following layers.

### 4.1 Demonstration and Integration Layer

Responsibilities:

- Construct and initialize application components
- Select PC mock implementations
- Provide demonstration input sequences
- Present diagnostic output
- Report application status and LED states

This layer is PC-specific in the demonstration build. A future embedded startup
module would perform an equivalent composition role on target hardware.

### 4.2 Application Layer

Responsibilities:

- Coordinate initialization
- Read and validate EEPROM configuration
- Select the Rev-A or Rev-B sensor conversion
- Coordinate acquisition and sample processing
- Manage application state
- Apply failure-handling policy
- Request LED-state changes

The application layer depends on abstractions rather than concrete PC mocks or
target drivers.

### 4.3 Domain Layer

Responsibilities:

- Convert sensor digits to degrees Celsius using the selected resolution
- Classify normalized temperature
- Map temperature conditions to logical LED states
- Represent statuses and domain values

The domain layer contains no direct ADC, EEPROM, timer, GPIO, console, or
operating-system access.

### 4.4 Hardware Abstraction Layer

Responsibilities:

- Define ADC operations
- Define EEPROM operations
- Define GPIO operations
- Define timer operations
- Hide PC-specific and target-specific implementation details

The C implementation uses explicit C interfaces and state objects. The C++
implementation uses abstract interfaces and dependency injection where
appropriate.

### 4.5 Hardware Implementation Layer

Two implementation families are anticipated:

1. PC mocks for functional demonstration and testing
2. Target drivers for a future bare-metal platform

Only the PC mocks are implemented in the current project scope.

## 5. Core Components

### 5.1 Application Controller

The application controller coordinates the system.

Initialization responsibilities:

1. Initialize GPIO access.
2. Request all LEDs off.
3. Initialize and read EEPROM configuration.
4. Validate the hardware revision.
5. Retain the bounded serial number.
6. Select the sensor conversion.
7. Initialize ADC acquisition.
8. Initialize the sampling mechanism.
9. Enter the monitoring state.

Runtime responsibilities:

1. Accept a raw sensor digit.
2. Normalize it according to the configured revision.
3. Classify the normalized temperature.
4. Select the required LED state.
5. Apply the LED state through the GPIO abstraction.
6. Report failures through application status.

### 5.2 Sensor Conversion

The sensor-conversion component converts the active sensor's digit into the
common fixed-point representation.

The common unit is one tenth of a degree Celsius.

Rev-A conversion:

    normalized_temperature = sensor_digit * 10

Rev-B conversion:

    normalized_temperature = sensor_digit

Conversion validates that the result can be represented without overflow.

### 5.3 Temperature Classifier

The classifier accepts a normalized temperature and returns one condition:

| Normalized range | Temperature range | Condition |
|---:|---|---|
| Below 50 | Below 5.0 degrees Celsius | Critical |
| 50 through 849 | 5.0 through 84.9 degrees Celsius | Normal |
| 850 through 1049 | 85.0 through 104.9 degrees Celsius | Warning |
| 1050 or greater | 105.0 degrees Celsius or greater | Critical |

The classifier has no dependency on the hardware revision because conversion
has already produced a common unit.

### 5.4 LED Controller

The LED controller translates the application condition into a logical GPIO
request:

| Condition | Green | Yellow | Red |
|---|---:|---:|---:|
| Normal | On | Off | Off |
| Warning | Off | On | Off |
| Critical | Off | Off | On |
| Fault | Off | Off | On |
| Initializing | Off | Off | Off |

Electrical active-high or active-low behavior belongs to the GPIO driver.

### 5.5 Configuration Provider

The configuration provider obtains:

- Hardware revision
- Hardware serial number

The application accepts only revision 0 or revision 1. It does not silently
substitute a default revision when configuration is invalid.

## 6. Execution Model

### 6.1 Embedded Target Model

The intended embedded acquisition period is 100 microseconds, corresponding to
10 kHz.

A hardware timer provides the periodic event. Depending on target ADC
capabilities, the preferred design is:

1. Hardware timer triggers ADC conversion.
2. ADC-complete interrupt captures the completed sample.
3. The interrupt handler publishes the sample with minimal processing.
4. Non-interrupt application processing performs conversion, classification,
   diagnostics, and LED policy.

If direct timer-to-ADC triggering is unavailable, the timer ISR may start the
conversion and an ADC-complete ISR may capture it.

The final target-specific mechanism requires review of the selected
microcontroller and ADC timing characteristics.

### 6.2 Interrupt-Service Routine Policy

Interrupt handlers shall:

- Perform only bounded, time-critical work
- Avoid formatted output
- Avoid dynamic allocation
- Avoid blocking operations
- Avoid executing the complete application policy
- Detect or expose sample overrun where practical

Shared ISR and foreground state requires target-appropriate synchronization.
The use of `volatile` alone is not assumed to provide atomicity or complete
synchronization.

### 6.3 Foreground Processing

The foreground loop or scheduler performs:

- Sensor normalization
- Temperature classification
- LED-state selection
- Error-policy execution
- Diagnostic-status updates

No operating system is required by the architecture.

### 6.4 PC Demonstration Model

The PC demonstration invokes acquisition and processing synchronously using
mock objects or mock modules.

It demonstrates:

- Configuration selection
- Sensor conversion
- Boundary classification
- LED transitions
- Failure handling
- Equivalent C and C++ behavior

It does not prove:

- 100-microsecond real-time scheduling
- Interrupt latency
- Sampling jitter
- ADC electrical behavior
- GPIO electrical behavior

## 7. Data Flow

The normal data flow is:

1. EEPROM provides revision and serial number during initialization.
2. Timer initiates an acquisition event.
3. ADC provides one sensor digit.
4. Sensor conversion produces a temperature in degrees Celsius.
5. The classifier produces normal, warning, or critical.
6. The LED controller produces mutually exclusive logical LED states.
7. GPIO applies the requested output state.

The failure flow is:

1. A component reports failure or invalid data.
2. The application rejects unreliable data.
3. The application records an explicit failure status.
4. The LED controller requests green off, yellow off, and red on.
5. The PC demonstration reports the specific failure.

## 8. Dependency Rules

The following dependency rules apply:

1. Domain logic shall not depend on hardware abstractions.
2. Domain logic shall not depend on PC demonstration facilities.
3. Application logic may depend on domain logic and hardware abstractions.
4. Application logic shall not depend directly on concrete PC mocks.
5. Hardware implementations may depend on their corresponding abstractions.
6. The composition root may depend on all components required to assemble the
   program.
7. Tests may depend directly on the component under test and test doubles.

Dependencies point inward toward stable policy and domain behavior.

## 9. C Implementation Approach

The C implementation will use:

- Header-defined module interfaces
- Explicit context structures where state is required
- Enumerations for status and temperature condition
- Functions for conversion and classification
- Function pointers only where runtime hardware substitution requires them
- Explicit initialization and error return values
- No required dynamic allocation

Global mutable state will be avoided except where a target ISR entry point
requires carefully controlled shared state.

## 10. C++ Implementation Approach

The C++ implementation will use:

- Encapsulated application and controller classes
- Abstract hardware interfaces
- Constructor-based dependency injection
- Separate Rev-A and Rev-B sensor-conversion strategies
- Strongly typed enumerations
- Deterministic object lifetimes
- No required dynamic allocation
- Small interfaces with clear ownership expectations

Inheritance will be used for substitutable interfaces, not as a default for all
components.

## 11. Failure Handling

When reliable classification is impossible, the application requests the safe
indicator state:

- Green off
- Yellow off
- Red on

The application also exposes a status that distinguishes configuration, ADC,
GPIO, timer, overrun, invalid-data, and internal errors.

Detailed policy is documented in `error-handling.md`.

## 12. Testability

The architecture supports tests at three levels.

### 12.1 Unit Tests

Unit tests verify:

- Rev-A normalization
- Rev-B normalization
- Conversion overflow handling
- Temperature classification boundaries
- LED-state mapping

### 12.2 Component Tests

Component tests verify:

- Configuration validation
- Revision selection
- Application state transitions
- ADC failure handling
- EEPROM failure handling
- GPIO requests

### 12.3 Integration Demonstration

Integration tests or demonstrations verify:

- End-to-end mock input sequences
- Diagnostic output
- Equivalent C and C++ behavior
- Shared test vectors

## 13. Traceability Summary

| Architectural element | Principal requirements |
|---|---|
| Timer-driven acquisition | REQ-FUNC-008 through REQ-FUNC-012 |
| Sensor conversion | REQ-FUNC-013 through REQ-FUNC-018 |
| Temperature classifier | REQ-FUNC-019 through REQ-FUNC-022 |
| LED controller | REQ-FUNC-023 through REQ-FUNC-027 |
| Error policy | REQ-ERR-001 through REQ-ERR-006 |
| Hardware abstraction | REQ-SW-004 and REQ-SW-005 |
| Independent domain tests | REQ-SW-006 |
| C/C++ equivalence | REQ-SW-007 |
| Architecture documentation | REQ-DOC-001 and REQ-DOC-002 |

## 14. Deferred Target-Specific Decisions

The following decisions require an actual embedded target:

- Timer peripheral selection and prescaler values
- ADC trigger and conversion timing
- ADC interrupt mechanism
- ISR vector names
- GPIO register mapping and polarity
- EEPROM driver protocol
- Atomicity and critical-section implementation
- Linker script and startup code
- Worst-case execution-time analysis
- Measured sampling jitter

These items are intentionally isolated from the hardware-independent domain and
application policies.
