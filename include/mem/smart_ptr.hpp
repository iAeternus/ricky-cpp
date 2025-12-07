/**
 * @brief 智能指针
 * @author Ricky
 * @date 2025/7/13
 * @version 1.0
 */
#ifndef SMART_PTR_HPP
#define SMART_PTR_HPP

#include "marker.hpp"

namespace my::mem {

// TODO 计划实现：
// Box<T>，可以将值分配到堆上，独占所有权
// Rc<T>，引用计数类型，允许多所有权存在
// Weak<T>，弱引用类型，允许引用计数类型的非所有权引用

/**
 * @class Box
 * @brief 独占所有权的智能指针
 * @brief 禁止拷贝，允许移动
 * @tparam T 值类型
 */
template <typename T>
class Box : public NoCopyMove {
public:
    using value_t = T;
    using Self = Box<value_t>;

    /**
     * @brief 构造函数
     * @param ptr 指向分配的值
     */
    explicit Box(value_t* ptr = nullptr) :
            ptr_(ptr) {}

    /**
     * @brief 移动构造函数
     * @param other 另一个Box
     */
    Box(Self&& other) noexcept :
            ptr_(other.ptr_) {
        other.ptr_ = nullptr;
    }

    /**
     * @brief 移动赋值运算符
     * @param other 另一个Box
     * @return 当前Box
     */
    Self& operator=(Self&& other) noexcept {
        if (this == &other) return *this;
    }

    ~Box();

    void reset(value_t* ptr = nullptr);

    value_t* get() const;

    value_t* release();

    value_t& operator*() const;
    value_t* operator->() const;

    /**
     * @brief 转换为bool，可检查指针是否为空
     */
    explicit operator bool() const {
        return ptr_ != nullptr;
    }

private:
    value_t* ptr_; // 指向分配的值
};


} // namespace my::mem

#endif // SMART_PTR_HPP