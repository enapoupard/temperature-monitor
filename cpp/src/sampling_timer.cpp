#include "temperature_monitor/sampling_timer.hpp"

#include <stdexcept>
#include <utility>

namespace temperature_monitor {

SamplingTimer::SamplingTimer(Callback callback)
    : callback_{std::move(callback)}
{
    if (!callback_) {
        throw std::invalid_argument{
            "SamplingTimer requires a valid callback"};
    }
}

void SamplingTimer::start() noexcept
{
    running_ = true;
}

void SamplingTimer::stop() noexcept
{
    running_ = false;
}

bool SamplingTimer::is_running() const noexcept
{
    return running_;
}

void SamplingTimer::irq_handler()
{
    if (running_) {
        callback_();
    }
}

} // namespace temperature_monitor
