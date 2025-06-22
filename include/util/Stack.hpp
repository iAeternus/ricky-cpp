/**
 * @brief 栈，动态数组实现
 * @author Ricky
 * @date 2025/1/13
 * @version 1.0
 */
#ifndef STACK_HPP
#define STACK_HPP

#include "Exception.hpp"
#include "Vec.hpp"

namespace my::util {

template <typename T>
class Stack : public Object<Stack<T>> {
public:
    using value_t = T;
    using Self = Stack<value_t>;

    Stack() :
            data_() {}

    usize size() const {
        return data_.size();
    }

    bool empty() const {
        return data_.empty();
    }

    /**
     * @brief 入栈
     */
    void push(const value_t& item) {
        data_.append(item);
    }

    /**
     * @brief 入栈（右值引用版本）
     */
    template <typename U>
    void push(U&& item) {
        data_.append(std::forward<U>(item));
    }

    /**
     * @brief 出栈
     */
    void pop() {
        data_.pop(-1);
    }

    /**
     * @brief 查看栈顶元素
     * @note 若栈空，则抛出 runtime_exception
     * @return 栈顶元素的引用
     */
    value_t& peek() {
        if (empty()) {
            runtime_exception("stack is empty.");
        }
        return data_.back();
    }

    /**
     * @brief 查看栈顶元素（常量版本）
     * @note 若栈空，则抛出 runtime_exception
     * @return 栈顶元素的引用
     */
    const value_t& peek() const {
        if (empty()) {
            runtime_exception("stack is empty.");
        }
        return data_.back();
    }

private:
    Vec<value_t> data_;
};

} // namespace my::util

#endif // STACK_HPP