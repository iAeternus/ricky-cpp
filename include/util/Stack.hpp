/**
 * @brief 栈，动态数组实现
 * @author Ricky
 * @date 2025/1/13
 * @version 1.0
 */
#ifndef STACK_HPP
#define STACK_HPP

#include "DynArray.hpp"

namespace my::util {

template <typename T>
class Stack {
public:
    using value_t = T;

    Stack() :
            data_({}) {}

    c_size size() const {
        return data_.size();
    }

    bool isEmpty() const {
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
     * @note 若栈空，则抛出ValueError
     * @return 栈顶元素的引用
     */
    T& peek() {
        if (isEmpty()) {
            ValueError("Stack is empty.");
            return None<T>;
        }
        return data_.back();
    }

    /**
     * @brief 查看栈顶元素（常量版本）
     * @note 若栈空，则抛出ValueError
     * @return 栈顶元素的引用
     */
    const T& peek() const {
        if (isEmpty()) {
            ValueError("Stack is empty.");
            return None<T&>;
        }
        return data_.back();
    }

private:
    DynArray<T> data_;
};

} // namespace my::util

#endif // STACK_HPP 