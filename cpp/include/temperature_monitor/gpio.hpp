#ifndef TEMPERATURE_MONITOR_GPIO_HPP
#define TEMPERATURE_MONITOR_GPIO_HPP

#include "temperature_monitor/temperature_classification.hpp"

#include <functional>

namespace temperature_monitor {

struct LedState {
    bool green{false};
    bool yellow{false};
    bool red{false};
};

class Gpio {
public:
    using WriteFunction = std::function<void(const LedState&)>;

    explicit Gpio(WriteFunction write_function);

    void set_temperature_status(TemperatureStatus status) const;

private:
    WriteFunction write_function_;
};

} // namespace temperature_monitor

#endif
