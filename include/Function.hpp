/**
 * @brief 基本函数式编程组件
 * @author Ricky
 * @date 2025/5/9
 * @version 1.0
 */
#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include "ricky.hpp"

#include <functional>

namespace my {

/**
 * @brief 行为函数
 * @note 无参无返回值的函数
 */
using Runnable = std::function<void()>;

/**
 * @brief 消费者
 */
template <typename T>
using Consumer = std::function<void(T)>;

/**
 * @brief 生产者
 */
template <typename T>
using Supplier = std::function<T()>;

/**
 * @class Pred
 * @brief 谓词函数封装
 * @details 单参数返回bool的函数
 */
template <typename T>
class Pred {
public:
    using Self = Pred<T>;

    /**
     * @brief 使用函数构造
     * @tparam F 函数类型，要求入参类型为T，返回值可隐式转化为bool
     * @param func 函数
     */
    template <typename F>
        requires std::invocable<F, T> && std::convertible_to<std::invoke_result_t<F, T>, bool>
    Pred(F&& func) :
            func_(std::forward<F>(func)) {}

    Pred(Self&&) noexcept = default;
    Self& operator=(Self&&) noexcept = default;

    explicit operator bool() const noexcept {
        return static_cast<bool>(func_);
    }

    /**
     * @brief 执行谓词
     * @param arg 谓词入参
     * @return 谓词结果
     */
    fn operator()(const T& arg) const {
        return func_(arg);
    }

    /**
     * @brief 执行谓词
     * @tparam Args 谓词入参类型
     * @param args 谓词入参
     * @return 谓词结果
     */
    template <typename... Args>
    fn operator()(Args&&... args) const {
        return func_(std::forward<Args>(args)...);
    }

    /**
     * @brief 谓词间与运算
     * @param other 另一个谓词
     * @return 新谓词
     */
    fn operator&&(const Self& other) const {
        return Self([*this, other](const T& arg) {
            return (*this)(arg) && other(arg);
        });
    }

    /**
     * @brief 谓词间或运算
     * @param other 另一个谓词
     * @return 新谓词
     */
    fn operator||(const Self& other) const {
        return Self([*this, other](const T& arg) {
            return (*this)(arg) || other(arg);
        });
    }

    /**
     * @brief 谓词取非
     * @return 新谓词
     */
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