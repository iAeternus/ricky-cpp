/**
 * @brief 基本函数式编程组件
 * @author Ricky
 * @date 2025/5/9
 * @version 1.0
 */
#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include <functional>

namespace my {

/**
 * @brief 行为函数
 * @note 无参无返回值的函数
 */
using Runnable = std::function<void(void)>;

/**
 * @brief 谓词函数
 * @note 单参数返回bool的函数
 */
template <typename T>
class Pred {
public:
    template <typename F, typename = std::enable_if_t<std::is_invocable_r_v<bool, F, T>>>
    Pred(F&& func) :
            func_(std::forward<F>(func)) {}

    Pred(Pred&&) noexcept = default;
    Pred& operator=(Pred&&) noexcept = default;

    explicit operator bool() const noexcept {
        return static_cast<bool>(func_);
    }

    bool operator()(const T& arg) const {
        return func_(arg);
    }

    template <typename... Args>
    bool operator()(Args&&... args) const {
        return func_(std::forward<Args>(args)...);
    }

    Pred operator&&(const Pred& other) const {
        return Pred([*this, other](const T& arg) {
            return (*this)(arg) && other(arg);
        });
    }

    Pred operator||(const Pred& other) const {
        return Pred([*this, other](const T& arg) {
            return (*this)(arg) || other(arg);
        });
    }

    Pred operator!() const {
        return Pred([*this](const T& arg) {
            return !(*this)(arg);
        });
    }

private:
    std::function<bool(T)> func_;
};

} // namespace my

#endif // FUNCTION_HPP