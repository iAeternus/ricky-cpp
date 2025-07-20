/**
 * @brief 独占所有权的智能指针，可以将值分配到堆上，被销毁时自动释放
 * @author Ricky
 * @date 2025/7/18
 * @version 1.0
 */
#ifndef BOX_HPP
#define BOX_HPP

#include "NoCopy.hpp"

namespace my::mem {

/**
 * @class Box
 * @brief 独占所有权的智能指针
 * @brief 禁止拷贝，允许移动
 * @tparam T 值类型
 */
template <typename T>
class Box : public NoCopyable {
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
        if(this == &other) return *this;

        
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

#endif // BOX_HPP