/**
 * @brief 高精度计时器
 * @author Ricky
 * @date 2024/11/27
 * @version 1.0
 */
#ifndef TIMER_HPP
#define TIMER_HPP

#include "object.hpp"

#include <chrono>

namespace my::util {

/**
 * @class Timer
 * @brief 高精度计时器
 * @tparam D 时间间隔类型
 */
template <typename D>
class Timer : public Object<Timer<D>> {
public:
    using Self = Timer<D>;

    void start() {
        start_time_ = std::chrono::high_resolution_clock::now();
    }

    long long end() {
        const auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<D>(end_time - start_time_).count();
        return duration;
    }

    template <typename F, typename... Args>
    long long operator()(F&& call, Args&&... args) {
        start();
        std::invoke(std::forward<F>(call), std::forward<Args>(args)...);
        return end();
    }

private:
    std::chrono::high_resolution_clock::time_point start_time_;
};

/**
 * @brief 对应时间间隔类型的计时器别名
 */
using Timer_s = Timer<std::chrono::seconds>;
using Timer_ms = Timer<std::chrono::milliseconds>;
using Timer_us = Timer<std::chrono::microseconds>;
using Timer_ns = Timer<std::chrono::nanoseconds>;

} // namespace my::util

#endif // TIMER_HPP