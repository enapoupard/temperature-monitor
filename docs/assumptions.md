# Assumptions and Design Interpretations

## 1. Purpose

This document records assumptions made where the original requirements are
ambiguous or do not completely define the expected behavior.

Assumptions are not treated as original stakeholder requirements. They are
design decisions that should be reviewed and updated if additional stakeholder
information becomes available.

## 2. Temperature Range Interpretation

### ASM-001 — Critical range priority

The statement that normal temperature is below 85 degrees Celsius overlaps with
the statement that temperatures below 5 degrees Celsius are critical.

The software interprets the intended ranges as:

- Below 5 degrees Celsius: critical
- At least 5 and below 85 degrees Celsius: normal
- At least 85 and below 105 degrees Celsius: warning
- At least 105 degrees Celsius: critical

Therefore, the critical classification has priority for temperatures below
5 degrees Celsius.

### ASM-002 — Exact boundaries

The exact boundary behavior is:

| Temperature | Classification |
|---|---|
| Below 5.0 degrees Celsius | Critical |
| Exactly 5.0 degrees Celsius | Normal |
| Immediately below 85.0 degrees Celsius | Normal |
| Exactly 85.0 degrees Celsius | Warning |
| Immediately below 105.0 degrees Celsius | Warning |
| Exactly 105.0 degrees Celsius | Critical |

## 3. Temperature Representation

### ASM-003 — Fixed-point representation

The common internal temperature unit is one tenth of a degree Celsius.

Examples:

| Temperature | Internal value |
|---:|---:|
| 5.0 degrees Celsius | 50 |
| 10.0 degrees Celsius | 100 |
| 85.0 degrees Celsius | 850 |
| 105.0 degrees Celsius | 1050 |

This avoids a dependency on floating-point arithmetic in the domain logic.

### ASM-004 — Rev-A normalization

A valid Rev-A digit is multiplied by 10 to obtain the common internal unit.

For example, a Rev-A digit of 10 becomes an internal value of 100, representing
10.0 degrees Celsius.

### ASM-005 — Rev-B normalization

A Rev-B digit is multiplied by 0.1 degrees Celsius per digit.

For example, a Rev-B digit of 100 represents 10.0 degrees Celsius.

### ASM-006 — ADC abstraction output

For this demonstration, the ADC abstraction returns the sensor digit described
by the requirements.

Any conversion from ADC voltage counts to the documented sensor digit is
considered hardware-specific and outside the current project scope.

## 4. Timing Model

### ASM-007 — Embedded sampling trigger

On a real embedded target, a hardware timer triggers temperature acquisition
every 100 microseconds.

### ASM-008 — Minimal interrupt processing

An embedded interrupt handler performs only time-critical work, such as
triggering or capturing ADC acquisition and publishing a sample-ready event.

Temperature classification and LED policy are performed outside the
time-critical interrupt context unless a target-specific timing analysis shows
that another design is necessary.

### ASM-009 — PC timing limitation

The PC demonstration is not used to claim compliance with hard real-time
sampling jitter.

It demonstrates interfaces, control flow, conversion, classification, LED
behavior, and failure handling.

### ASM-010 — Every accepted sample is processed

The initial design processes each accepted sample in order. If target execution
cannot keep pace with the 10 kHz acquisition rate, this is treated as an
overrun condition rather than silently assuming that samples may be discarded.

## 5. LED Behavior

### ASM-011 — Active-high logical model

The application uses logical LED states of on and off.

Whether a physical GPIO is electrically active-high or active-low is handled by
the target-specific GPIO implementation.

### ASM-012 — One active condition LED

For a valid temperature classification, exactly one of the green, yellow, and
red LEDs is active.

### ASM-013 — Startup behavior

All LEDs are initially off while hardware and configuration are being
initialized.

After successful initialization, the first valid sample determines the
displayed condition.

If initialization fails, the application requests the safe error indication:
red on, green off, and yellow off.

### ASM-014 — No hysteresis

The initial implementation applies the thresholds directly and does not add
hysteresis.

A temperature oscillating around a boundary may therefore cause the indicated
condition to change repeatedly.

### ASM-015 — No filtering

The initial implementation does not average, debounce, or filter temperature
samples because no filtering requirement was provided.

## 6. EEPROM Configuration

### ASM-016 — Configuration read timing

The hardware revision and serial number are read once during application
initialization.

Runtime changes to EEPROM configuration are not supported.

### ASM-017 — Revision values

The only valid hardware revision values are:

- 0 for Rev-A
- 1 for Rev-B

All other values are invalid.

### ASM-018 — Serial number role

The serial number is used only for identification or demonstration output and
does not affect temperature conversion or classification.

### ASM-019 — Example serial number

The value `ABC1234` is demonstration data rather than a required fixed serial
number.

### ASM-020 — Serial number storage

The serial number is represented as a bounded, null-terminated character
string. Its final maximum length will be defined in the software interface.

## 7. Failure Behavior

### ASM-021 — Fail-safe indicator

If configuration or sensor failure prevents reliable temperature
classification, the logical LED request is:

- Green off
- Yellow off
- Red on

The red LED therefore indicates either a critical temperature or a detected
system error. The three-LED interface alone does not distinguish those causes.

### ASM-022 — Error reporting

In addition to the LEDs, software status values and PC demonstration output are
used to distinguish failure causes.

### ASM-023 — Recovery policy

The initial implementation enters a fault state after an unrecoverable
initialization failure.

For a runtime ADC failure, the initial implementation requests the safe
indicator state and reports the failure. Automatic retry behavior will be
defined as part of the application state-machine design.

## 8. Software Structure

### ASM-024 — Separate implementations

The C and C++ versions are separate implementations of the same behavior rather
than one implementation wrapping the other.

### ASM-025 — Shared behavioral specification

Both implementations use the same requirement boundaries and shared test data
where practical.

### ASM-026 — Build system

CMake is used as the portable build-system generator for the PC demonstration
and tests.

### ASM-027 — No dynamic allocation requirement

The domain and application design should not require dynamic memory allocation.
This supports later use on a bare-metal target.

## 9. Open Questions

The following points are not fully specified and are retained for review:

1. What sensor-digit ranges are physically valid for Rev-A and Rev-B?
2. How are raw ADC conversion counts transformed into sensor digits on the real
   hardware?
3. What is the required maximum sampling jitter?
4. What is the maximum permitted processing latency after acquisition?
5. Must every sample update the GPIO outputs, or may LED updates occur at a
   slower rate?
6. Is temperature filtering or threshold hysteresis expected?
7. Should a runtime ADC failure be retried automatically?
8. Is there a separate indication required to distinguish a hardware fault from
   a critical temperature?
9. What is the maximum EEPROM serial-number length?
10. Is EEPROM integrity protection, such as a checksum or CRC, required?
11. What should happen if sample processing cannot keep up with acquisition?
12. Are diagnostic counters or fault history required?

Until these questions are resolved, the documented assumptions define the
project behavior.
