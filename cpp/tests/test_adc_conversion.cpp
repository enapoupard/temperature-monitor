#include "temperature_monitor/adc_conversion.hpp"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>

namespace tempmon = temperature_monitor;

namespace {

[[nodiscard]] bool nearly_equal(double left, double right) noexcept
{
    return std::fabs(left - right) < 1.0e-12;
}

void test_sensor_configuration()
{
    const auto* rev_a =
        tempmon::sensor_config(tempmon::SensorRevision::rev_a);
    const auto* rev_b =
        tempmon::sensor_config(tempmon::SensorRevision::rev_b);

    assert(rev_a != nullptr);
    assert(rev_b != nullptr);

    assert(rev_a->revision == tempmon::SensorRevision::rev_a);
    assert(nearly_equal(rev_a->degrees_c_per_digit, 1.0));

    assert(rev_b->revision == tempmon::SensorRevision::rev_b);
    assert(nearly_equal(rev_b->degrees_c_per_digit, 0.1));

    const auto invalid_revision =
        static_cast<tempmon::SensorRevision>(2);

    assert(tempmon::sensor_config(invalid_revision) == nullptr);
}

void test_requirement_examples()
{
    const auto& rev_a =
        *tempmon::sensor_config(tempmon::SensorRevision::rev_a);
    const auto& rev_b =
        *tempmon::sensor_config(tempmon::SensorRevision::rev_b);

    const auto rev_a_temperature =
        tempmon::adc_to_temperature(10U, rev_a);
    const auto rev_b_temperature =
        tempmon::adc_to_temperature(100U, rev_b);

    assert(rev_a_temperature.has_value());
    assert(rev_b_temperature.has_value());
    assert(nearly_equal(*rev_a_temperature, 10.0));
    assert(nearly_equal(*rev_b_temperature, 10.0));

    const auto rev_a_digit =
        tempmon::temperature_to_adc(10.0, rev_a);
    const auto rev_b_digit =
        tempmon::temperature_to_adc(10.0, rev_b);

    assert(rev_a_digit == std::optional<std::uint32_t>{10U});
    assert(rev_b_digit == std::optional<std::uint32_t>{100U});
}

void test_boundary_round_trips()
{
    const auto& rev_a =
        *tempmon::sensor_config(tempmon::SensorRevision::rev_a);
    const auto& rev_b =
        *tempmon::sensor_config(tempmon::SensorRevision::rev_b);

    constexpr double temperatures[] {
        0.0,
        4.0,
        5.0,
        84.0,
        85.0,
        105.0,
        120.0
    };

    for (const double expected : temperatures) {
        const auto rev_a_digit =
            tempmon::temperature_to_adc(expected, rev_a);
        assert(rev_a_digit.has_value());

        const auto rev_a_temperature =
            tempmon::adc_to_temperature(*rev_a_digit, rev_a);
        assert(rev_a_temperature.has_value());
        assert(nearly_equal(*rev_a_temperature, expected));

        const auto rev_b_digit =
            tempmon::temperature_to_adc(expected, rev_b);
        assert(rev_b_digit.has_value());

        const auto rev_b_temperature =
            tempmon::adc_to_temperature(*rev_b_digit, rev_b);
        assert(rev_b_temperature.has_value());
        assert(nearly_equal(*rev_b_temperature, expected));
    }
}

void test_quantization()
{
    const auto& rev_a =
        *tempmon::sensor_config(tempmon::SensorRevision::rev_a);
    const auto& rev_b =
        *tempmon::sensor_config(tempmon::SensorRevision::rev_b);

    const auto rev_a_digit =
        tempmon::temperature_to_adc(84.6, rev_a);
    assert(rev_a_digit == std::optional<std::uint32_t>{85U});

    const auto rev_a_temperature =
        tempmon::adc_to_temperature(*rev_a_digit, rev_a);
    assert(rev_a_temperature.has_value());
    assert(nearly_equal(*rev_a_temperature, 85.0));

    const auto rev_b_digit =
        tempmon::temperature_to_adc(84.96, rev_b);
    assert(rev_b_digit == std::optional<std::uint32_t>{850U});

    const auto rev_b_temperature =
        tempmon::adc_to_temperature(*rev_b_digit, rev_b);
    assert(rev_b_temperature.has_value());
    assert(nearly_equal(*rev_b_temperature, 85.0));
}

void test_invalid_values()
{
    const auto& rev_a =
        *tempmon::sensor_config(tempmon::SensorRevision::rev_a);

    const tempmon::SensorConfig invalid_scale {
        tempmon::SensorRevision::rev_a,
        0.0
    };

    const tempmon::SensorConfig invalid_revision {
        static_cast<tempmon::SensorRevision>(2),
        1.0
    };

    assert(!tempmon::temperature_to_adc(-0.1, rev_a).has_value());
    assert(!tempmon::temperature_to_adc(
        std::numeric_limits<double>::quiet_NaN(),
        rev_a).has_value());
    assert(!tempmon::temperature_to_adc(
        std::numeric_limits<double>::infinity(),
        rev_a).has_value());

    assert(!tempmon::adc_to_temperature(
        10U,
        invalid_scale).has_value());

    assert(!tempmon::temperature_to_adc(
        10.0,
        invalid_revision).has_value());
}

} // namespace

int main()
{
    test_sensor_configuration();
    test_requirement_examples();
    test_boundary_round_trips();
    test_quantization();
    test_invalid_values();

    std::cout << "C++ ADC conversion tests passed.\n";
    return 0;
}
