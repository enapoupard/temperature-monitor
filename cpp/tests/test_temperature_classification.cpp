#include "temperature_monitor/temperature_classification.hpp"

#include <cassert>
#include <iostream>
#include <limits>
#include <string_view>

namespace tempmon = temperature_monitor;

namespace {

void test_critical_low_temperature()
{
    assert(tempmon::classify_temperature(-1.0) ==
           tempmon::TemperatureStatus::red);

    assert(tempmon::classify_temperature(0.0) ==
           tempmon::TemperatureStatus::red);

    assert(tempmon::classify_temperature(4.9) ==
           tempmon::TemperatureStatus::red);
}

void test_normal_temperature()
{
    assert(tempmon::classify_temperature(5.0) ==
           tempmon::TemperatureStatus::green);

    assert(tempmon::classify_temperature(20.0) ==
           tempmon::TemperatureStatus::green);

    assert(tempmon::classify_temperature(84.9) ==
           tempmon::TemperatureStatus::green);
}

void test_warning_temperature()
{
    assert(tempmon::classify_temperature(85.0) ==
           tempmon::TemperatureStatus::yellow);

    assert(tempmon::classify_temperature(100.0) ==
           tempmon::TemperatureStatus::yellow);

    assert(tempmon::classify_temperature(104.9) ==
           tempmon::TemperatureStatus::yellow);
}

void test_critical_high_temperature()
{
    assert(tempmon::classify_temperature(105.0) ==
           tempmon::TemperatureStatus::red);

    assert(tempmon::classify_temperature(120.0) ==
           tempmon::TemperatureStatus::red);
}

void test_invalid_temperature()
{
    assert(tempmon::classify_temperature(
               std::numeric_limits<double>::quiet_NaN()) ==
           tempmon::TemperatureStatus::invalid);

    assert(tempmon::classify_temperature(
               std::numeric_limits<double>::infinity()) ==
           tempmon::TemperatureStatus::invalid);

    assert(tempmon::classify_temperature(
               -std::numeric_limits<double>::infinity()) ==
           tempmon::TemperatureStatus::invalid);
}

void test_status_names()
{
    assert(std::string_view{tempmon::temperature_status_name(
               tempmon::TemperatureStatus::green)} == "NORMAL");

    assert(std::string_view{tempmon::temperature_status_name(
               tempmon::TemperatureStatus::yellow)} == "WARNING");

    assert(std::string_view{tempmon::temperature_status_name(
               tempmon::TemperatureStatus::red)} == "CRITICAL");

    assert(std::string_view{tempmon::temperature_status_name(
               tempmon::TemperatureStatus::invalid)} == "INVALID");

    const auto unknown_status =
        static_cast<tempmon::TemperatureStatus>(99);

    assert(std::string_view{
               tempmon::temperature_status_name(unknown_status)} ==
           "INVALID");
}

} // namespace

int main()
{
    test_critical_low_temperature();
    test_normal_temperature();
    test_warning_temperature();
    test_critical_high_temperature();
    test_invalid_temperature();
    test_status_names();

    std::cout << "C++ temperature classification tests passed.\n";
    return 0;
}
