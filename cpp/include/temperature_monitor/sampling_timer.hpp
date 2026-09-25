#ifndef TEMPERATURE_MONITOR_SAMPLING_TIMER_HPP
#define TEMPERATURE_MONITOR_SAMPLING_TIMER_HPP

#include <chrono>
#include <cstdint>
#include <functional>

namespace temperature_monitor {

class SamplingTimer {
public:
    using Callback = std::function<void()>;

    static constexpr auto sample_period =
        std::chrono::microseconds{100};

    static constexpr std::uint32_t sample_rate_hz = 10'000U;

    explicit SamplingTimer(Callback callback);

    void start() noexcept;
    void stop() noexcept;

    [[nodiscard]] bool is_running() const noexcept;

    /*
     * Represents one hardware timer interrupt.
     * The callback runs only while the timer is started.
     */
    void irq_handler();

private:
    Callback callback_;
    bool running_{false};
};

} // namespace temperature_monitor

#endif
