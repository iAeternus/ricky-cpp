/**
 * @brief 自动管理内存释放
 * @author Ricky
 * @date 2025/5/18
 * @version 1.0
 */
#ifndef DELETER_HPP
#define DELETER_HPP

#include "ricky_memory.hpp"

namespace my::util {

template <typename T>
struct Deleter : public Object<Deleter<T>> {
public:
    using value_t = T;
    using Self = Deleter<value_t>;

    /**
     * @brief 安全析构对象并释放内存
     * @param ptr 指向对象的指针
     */
    fn operator()(T* ptr) const noexcept {
        if (ptr != nullptr) {
            my::my_destroy(ptr);
            my::my_delloc(ptr);
        }
    }
};

/**
 * @brief 特化版本，用于处理动态数组（需显式传递数组大小）
 * @tparam T 数组元素类型
 */
template <typename T>
struct Deleter<T[]> : public Object<Deleter<T>> {
public:
    using value_t = T;
    using Self = Deleter<value_t>;

    /**
     * @brief 安全析构对象并释放内存
     * @param ptr 数组首地址
     * @param size 数组长度
     */
    fn operator()(T* ptr, size_t size) const noexcept {
        if (ptr != nullptr) {
            my::my_destroy(ptr, size);
            my::my_delloc(ptr);
        }
    }
};

} // namespace my::util

#endif // DELETER_HPP