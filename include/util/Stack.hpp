/**
 * @brief 栈，动态数组实现
 * @author Ricky
 * @date 2025/1/13
 * @version 1.0
 */
#ifndef STACK_HPP
#define STACK_HPP

#include "Vec.hpp"

namespace my::util {

/**
 * @class Stack
 * @brief 栈容器
 * @tparam T 元素类型
 * @tparam Alloc 内存分配器
 */
template <typename T, typename Alloc = mem::Allocator<T>>
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
     * @return 栈顶元素的引用
     * @exception Exception 若栈空，则抛出 runtime_exception
     */
    value_t& peek() {
        if (empty()) {
            throw runtime_exception("Stack is empty.");
        }
        return data_.back();
    }

    /**
     * @brief 查看栈顶元素（常量版本）
     * @return 栈顶元素的引用
     * @exception Exception 若栈空，则抛出 runtime_exception
     */
    const value_t& peek() const {
        if (empty()) {
            throw runtime_exception("stack is empty.");
        }
        return data_.back();
    }

private:
    Vec<value_t, Alloc> data_;
};

} // namespace my::util

#endif // STACK_HPP