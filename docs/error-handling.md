# Error-Handling Strategy

## 1. Purpose

This document defines the initial error-handling policy for the C and C++
implementations of the temperature monitor.

The objectives are to:

- Avoid displaying a misleading normal or warning state
- Make failures observable in tests and the PC demonstration
- Keep hardware-independent policy separate from hardware-specific detection
- Provide deterministic behavior suitable for a bare-metal design

## 2. Error Categories

### 2.1 Initialization Errors

Initialization errors occur before normal temperature monitoring begins.

Examples include:

- EEPROM initialization failure
- EEPROM read failure
- Invalid hardware revision
- Invalid or unterminated serial-number data
- ADC initialization failure
- GPIO initialization failure
- Timer initialization failure

### 2.2 Runtime Errors

Runtime errors occur after successful initialization.

Examples include:

- ADC acquisition failure
- Invalid sensor data
- Sample-buffer overrun
- Unexpected application state
- GPIO update failure, if supported by the target interface

### 2.3 Configuration Errors

Configuration errors occur when EEPROM data can be read but is not accepted.

Examples include:

- Hardware revision is neither 0 nor 1
- Serial-number representation is invalid
- Future configuration integrity validation fails

## 3. Safe Indicator Policy

When reliable temperature classification is not possible, the application
requests this logical LED state:

| LED | Requested state |
|---|---|
| Green | Off |
| Yellow | Off |
| Red | On |

This policy prevents a known failure from continuing to display normal or
warning operation.

The GPIO hardware abstraction remains responsible for translating logical LED
states into electrical pin levels.

## 4. Status Model

The application shall expose an explicit status rather than relying only on the
LED state.

The initial status categories are:

| Status | Meaning |
|---|---|
| OK | Operation completed successfully |
| EEPROM error | Configuration could not be read |
| Invalid configuration | Configuration was read but rejected |
| ADC error | Temperature acquisition failed |
| Invalid sensor data | Sensor data could not be accepted |
| Timer error | Sampling timer could not be configured |
| GPIO error | Requested indicator state could not be applied |
| Overrun | A new sample arrived before previous data was safely handled |
| Internal error | An unexpected software state was detected |

The exact C enumeration and C++ enumeration will be defined during interface
design.

## 5. Initialization Failure Policy

Initialization follows a deterministic sequence:

1. Initialize the GPIO abstraction.
2. Request all LEDs off.
3. Initialize and read EEPROM configuration.
4. Validate the hardware revision and serial-number representation.
5. Select the sensor conversion strategy.
6. Initialize the ADC abstraction.
7. Initialize the sampling mechanism.
8. Enter the monitoring state.

If a step fails:

1. Record the corresponding software status.
2. Do not enter normal monitoring.
3. Request the safe indicator state when GPIO control is available.
4. Return or expose the failure to the PC demonstration.
5. Avoid using configuration or sensor data that has not been validated.

If GPIO initialization itself fails, the software still records and reports the
failure, but it cannot guarantee a physical LED state.

## 6. Runtime ADC Failure Policy

When ADC acquisition fails:

1. Reject the failed sample.
2. Do not classify it as a valid temperature.
3. Record an ADC error status.
4. Request the safe indicator state.
5. Ensure that a previous green or yellow state is not intentionally retained.
6. Report the failure through the PC demonstration or diagnostic interface.

The retry and recovery policy will be finalized with the application state
machine. Tests shall not assume successful automatic recovery until that policy
has been documented.

## 7. Invalid Sensor Data Policy

A sensor value is invalid when:

- The ADC abstraction explicitly reports invalid data
- Conversion to the common internal unit would overflow
- The value falls outside a supported sensor domain once that domain has been
  defined
- Associated sample metadata is inconsistent

For invalid data:

1. Do not classify the value.
2. Record an invalid-sensor-data status.
3. Request the safe indicator state.
4. Preserve diagnostic information where practical.

The currently provided requirements do not define physical minimum and maximum
sensor digits. Range validation will therefore initially focus on data type and
conversion safety until valid sensor limits are specified.

## 8. Invalid Hardware Revision Policy

If EEPROM returns a hardware revision other than 0 or 1:

1. Report invalid configuration.
2. Do not select a sensor conversion.
3. Do not start normal temperature monitoring.
4. Request the safe indicator state.
5. Include the rejected revision value in PC diagnostic output where practical.

The software shall not silently default to Rev-A or Rev-B.

## 9. Sample Overrun Policy

A sample overrun occurs when the acquisition mechanism produces data faster
than the application can safely consume it.

The architecture shall make overruns detectable, for example through a pending
sample count, sequence counter, or bounded buffer status.

When an overrun is detected:

1. Record an overrun status or diagnostic counter.
2. Do not silently claim that all samples were processed.
3. Request the safe indicator state unless a later reviewed requirement defines
   another policy.
4. Preserve enough information to support diagnosis in the PC demonstration.

The final mechanism depends on the selected timer and ADC interaction model.

## 10. LED Update Failure Policy

If the GPIO abstraction can report output failures:

1. Record a GPIO error.
2. Attempt to request the safe indicator state only when doing so is meaningful.
3. Report that the requested physical indication cannot be guaranteed.

A software request for the red LED does not prove that the physical LED is
operating correctly.

## 11. Assertions and Defensive Checks

Assertions may be used during development to detect programmer errors, but they
shall not be the only handling mechanism for expected runtime failures.

Expected errors such as invalid EEPROM data or ADC acquisition failure shall be
represented by explicit return values or status values.

Public interfaces shall validate pointers, ranges, and state where appropriate.

## 12. Logging and PC Demonstration Output

The bare-metal architecture does not assume that a logging service exists.

For the PC demonstration, diagnostic output may include:

- Hardware revision
- Hardware serial number
- Raw sensor digit
- Normalized temperature
- Temperature classification
- Requested LED state
- Application status
- Fault description

Diagnostic output is demonstration support and shall remain separate from the
core temperature-classification logic.

## 13. Test Expectations

Tests shall verify at least:

- Invalid hardware revision is rejected
- EEPROM failure prevents monitoring
- ADC failure does not produce a valid classification
- Conversion overflow is rejected
- Failure requests red on, green off, and yellow off
- A stale green or yellow indication is not retained after a detected failure
- C and C++ implementations apply equivalent failure policies
- Error status identifies the detected failure category

## 14. Limitations

The PC demonstration cannot verify:

- Physical LED operation
- Electrical GPIO polarity
- Real ADC behavior
- EEPROM electrical behavior
- Hardware timer accuracy
- Interrupt latency
- Sampling jitter
- Physical sensor plausibility

Those properties require target hardware, target drivers, and target-specific
verification.
