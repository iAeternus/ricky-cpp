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
    using Self = Pred<T>;

    template <typename F>
    requires std::invocable<F, T> && std::convertible_to<std::invoke_result_t<F, T>, bool>
    Pred(F&& func) :
            func_(std::forward<F>(func)) {}

    Pred(Self&&) noexcept = default;
    Self& operator=(Self&&) noexcept = default;

    explicit operator bool() const noexcept {
        return static_cast<bool>(func_);
    }

    fn operator()(const T& arg) const {
        return func_(arg);
    }

    template <typename... Args>
    fn operator()(Args&&... args) const {
        return func_(std::forward<Args>(args)...);
    }

    fn operator&&(const Self& other) const {
        return Self([*this, other](const T& arg) {
            return (*this)(arg) && other(arg);
        });
    }

    fn operator||(const Self& other) const {
        return Self([*this, other](const T& arg) {
            return (*this)(arg) || other(arg);
        });
    }

    fn operator!() const {
        return Self([*this](const T& arg) {
            return !(*this)(arg);
        });
    }

private:
    std::function<bool(T)> func_;
};

} // namespace my

#endif // FUNCTION_HPP