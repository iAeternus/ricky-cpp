/**
 * @brief 协程工具
 * @author Ricky
 * @date 2025/1/31
 * @version 1.0
 */
#ifndef CO_UTILS_HPP
#define CO_UTILS_HPP

#include "Object.hpp"

#include <coroutine>

namespace my::coro {

/**
 * @brief 协程句柄
 */
using Coroutine = std::coroutine_handle<>;

struct CurrentCoro : std::suspend_always {
    /**
     * @brief 协程挂起时调用
     */
    Coroutine await_suspend(Coroutine coroutine) noexcept {
        previous_coro_ = coroutine;
        return coroutine;
    }

    /**
     * @brief 协程恢复时调用
     */
    Coroutine await_resume() const noexcept {
        return previous_coro_;
    }

private:
    Coroutine previous_coro_ = nullptr; // 存储当前协程挂起前的句柄
};

struct CoroAwaiter : std::suspend_always, Object<CoroAwaiter> {
    Coroutine coro_ = nullptr;

    CoroAwaiter() noexcept = default;

    CoroAwaiter(Coroutine coro) noexcept :
            coro_(coro) {}

    /**
     * @brief 协程挂起时调用
     */
    Coroutine await_suspend(Coroutine coro) const noexcept {
        (void)coro;
        return coro_ ? coro_ : std::noop_coroutine();
    }
};

} // namespace my::coro

#endif // CO_UTILS_HPP