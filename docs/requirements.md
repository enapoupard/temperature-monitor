# Software Requirements

## 1. Purpose

This document records the software requirements for the temperature monitoring
and visualization device.

The requirement identifiers defined here will be referenced by architecture
documentation, source code, and tests where appropriate.

## 2. Scope

The software monitors a temperature sensor connected through an ADC and
visualizes the current temperature condition using three LEDs connected through
GPIOs.

The project provides:

- A procedural implementation written in C
- An object-oriented implementation written in C++
- Mocked hardware interfaces for execution on a PC
- An architecture suitable for later adaptation to a bare-metal target

The PC demonstration validates functional behavior and software architecture.
It does not demonstrate hard real-time performance on an embedded target.

## 3. Functional Requirements

### 3.1 Initialization and Configuration

**REQ-FUNC-001 — System initialization**

The software shall initialize all required application components and hardware
abstractions before temperature monitoring begins.

**REQ-FUNC-002 — EEPROM configuration**

The software shall obtain the hardware revision and hardware serial number
through an EEPROM abstraction.

**REQ-FUNC-003 — Hardware revision selection**

The software shall select the active sensor conversion according to the
hardware revision read from EEPROM.

**REQ-FUNC-004 — Rev-A selection**

An EEPROM hardware revision value of 0 shall select the Rev-A sensor
conversion.

**REQ-FUNC-005 — Rev-B selection**

An EEPROM hardware revision value of 1 shall select the Rev-B sensor
conversion.

**REQ-FUNC-006 — Serial number availability**

The software shall read and retain the hardware serial number for
identification or demonstration output.

### 3.2 Temperature Acquisition

**REQ-FUNC-007 — ADC interface**

The software shall acquire temperature sensor digits through an ADC
abstraction.

**REQ-FUNC-008 — Sampling period**

On an embedded target, temperature acquisition shall be initiated every
100 microseconds.

**REQ-FUNC-009 — Sampling frequency**

The nominal sampling frequency shall be 10 kHz.

**REQ-FUNC-010 — Low-jitter design**

The embedded architecture shall use a hardware-timer-driven acquisition model
to minimize sampling jitter.

**REQ-FUNC-011 — ISR definition**

Interrupt service routine entry points required by the embedded architecture
shall be declared and implemented.

**REQ-FUNC-012 — PC ISR execution**

The PC demonstration shall not be required to invoke interrupt service routines
asynchronously.

### 3.3 Sensor Conversion

**REQ-FUNC-013 — Rev-A resolution**

For Rev-A, one sensor digit shall represent 1 degree Celsius.

**REQ-FUNC-014 — Rev-A example**

For Rev-A, a sensor value of 10 shall represent 10 degrees Celsius.

**REQ-FUNC-015 — Rev-B resolution**

For Rev-B, one sensor digit shall represent 0.1 degrees Celsius.

**REQ-FUNC-016 — Rev-B example**

For Rev-B, a sensor value of 100 shall represent 10 degrees Celsius.

**REQ-FUNC-017 — Single active sensor type**

Only the sensor type selected by the configured hardware revision shall be
operational during a program execution.

**REQ-FUNC-018 — Common internal unit**

Both sensor revisions shall be converted to a common internal temperature unit
before temperature classification.

### 3.4 Temperature Classification

**REQ-FUNC-019 — Low critical condition**

A temperature below 5 degrees Celsius shall be classified as critical.

**REQ-FUNC-020 — Normal condition**

A temperature greater than or equal to 5 degrees Celsius and below 85 degrees
Celsius shall be classified as normal.

**REQ-FUNC-021 — Warning condition**

A temperature greater than or equal to 85 degrees Celsius and below 105 degrees
Celsius shall be classified as warning.

**REQ-FUNC-022 — High critical condition**

A temperature greater than or equal to 105 degrees Celsius shall be classified
as critical.

### 3.5 LED Visualization

**REQ-FUNC-023 — Normal indication**

The green LED shall indicate the normal condition.

**REQ-FUNC-024 — Warning indication**

The yellow LED shall indicate the warning condition.

**REQ-FUNC-025 — Critical indication**

The red LED shall indicate the critical condition.

**REQ-FUNC-026 — Mutually exclusive indication**

During valid operation, no more than one condition LED shall be active at a
time.

**REQ-FUNC-027 — Classification update**

After processing a valid temperature sample, the software shall update the LED
state to represent the resulting temperature condition.

## 4. Error-Handling Requirements

**REQ-ERR-001 — Invalid hardware revision**

A hardware revision other than 0 or 1 shall be treated as an invalid
configuration.

**REQ-ERR-002 — EEPROM read failure**

The application shall detect and report an EEPROM read failure through its
software status interface.

**REQ-ERR-003 — ADC acquisition failure**

The application shall detect and report an ADC acquisition failure through its
software status interface.

**REQ-ERR-004 — Safe indicator state**

When a failure prevents reliable temperature classification, the application
shall deactivate the green and yellow LEDs and activate the red LED.

**REQ-ERR-005 — Invalid sensor data**

Sensor data that cannot be represented or is outside the supported input domain
shall be treated as invalid.

**REQ-ERR-006 — No stale normal indication**

The application shall not continue to display a normal or warning indication
after detecting a failure that makes the current temperature state unreliable.

## 5. Software and Design Requirements

**REQ-SW-001 — C implementation**

The first implementation shall be written in C.

**REQ-SW-002 — C++ implementation**

The second implementation shall be written in C++.

**REQ-SW-003 — Object-oriented design**

The C++ implementation shall apply object-oriented design principles.

**REQ-SW-004 — Hardware abstraction**

Application and domain logic shall not directly depend on PC-specific or
target-specific hardware implementations.

**REQ-SW-005 — Mocked PC interfaces**

The project shall provide mocked hardware interfaces sufficient to demonstrate
the software on a PC.

**REQ-SW-006 — Testable domain logic**

Sensor conversion, temperature classification, and LED-state selection shall
be testable independently of real hardware.

**REQ-SW-007 — Equivalent behavior**

The C and C++ implementations shall produce equivalent classifications and LED
states for equivalent configuration and sensor input.

## 6. Documentation Requirements

**REQ-DOC-001 — Architecture documentation**

The software architecture shall be documented.

**REQ-DOC-002 — Architecture diagram**

At least one architecture diagram shall be provided in PDF, PNG, or SVG format.

**REQ-DOC-003 — Build instructions**

The repository shall contain instructions for configuring and building both
implementations.

**REQ-DOC-004 — Test instructions**

The repository shall contain instructions for executing the available tests.

**REQ-DOC-005 — Design limitations**

The documentation shall distinguish PC functional demonstration behavior from
real embedded timing guarantees.

## 7. Repository Requirements

**REQ-REP-001 — Version control**

The complete project shall be maintained in a Git repository.

**REQ-REP-002 — Public repository**

The completed project, including architecture diagrams, shall be published as a
public GitHub repository after its contents have been reviewed for publication.

## 8. Temperature Classification Table

| Temperature range | Classification | LED |
|---|---|---|
| Below 5 degrees Celsius | Critical | Red |
| At least 5 and below 85 degrees Celsius | Normal | Green |
| At least 85 and below 105 degrees Celsius | Warning | Yellow |
| At least 105 degrees Celsius | Critical | Red |

The low critical requirement takes precedence over the original general
statement that normal operation is below 85 degrees Celsius.

## 9. Verification Overview

The planned verification methods are:

| Requirement area | Primary verification method |
|---|---|
| Sensor conversion | Unit tests |
| Temperature boundaries | Unit tests |
| LED mapping | Unit tests |
| Hardware revision selection | Unit and integration tests |
| Failure handling | Unit and integration tests |
| C and C++ equivalence | Shared test vectors |
| Architecture | Documentation review |
| Sampling design | Architecture and code review |
| Embedded jitter | Not measurable in the PC demonstration |
