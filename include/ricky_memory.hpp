/**
 * @brief 内存分配与回收工具
 * @author Ricky
 * @date 2024/11/24
 * @version 1.0
 */
#ifndef RICKY_MEMORY_HPP
#define RICKY_MEMORY_HPP

#include "ricky.hpp"

#include <memory>

namespace my {

/**
 * @brief 分配连续size个T的内存，不会调用T的构造函数
 * @param size 分配多少个T的内存
 * @return 指向这段连续空间首地址的指针
 */
template <typename T>
fn my_alloc(size_t size)->T* {
    if (size == 0) return nullptr;
    return static_cast<T*>(::operator new(sizeof(T) * size));
}

/**
 * @brief 在ptr上调用构造函数，并返回ptr
 * @param ptr 数组首地址
 * @param args 构造函数的参数包
 * @return 返回构造好的ptr
 */
template <typename T, typename... Args>
fn my_construct(T* ptr, Args&&... args)->T* {
    ::new (ptr) T(std::forward<Args>(args)...);
    return ptr;
}

/**
 * @brief 调用ptr的析构函数，不会释放内存
 * @param ptr 数组首地址
 * @param size 数组长度，默认为1
 * @return void
 */
template <typename T>
fn my_destroy(T* ptr, size_t size = 1) {
    if (ptr == nullptr) return;
    for (size_t i = 0; i < size; ++i, ++ptr) {
        ptr->~T();
    }
}

/**
 * @brief 释放ptr指向的内存
 * @param ptr 数组首地址
 * @return void
 */
template <typename T>
fn my_delloc(T*& ptr) {
    ::operator delete(ptr);
    ptr = nullptr;
}

template <typename T>
struct Deleter {
public:
    void operator()(T* ptr) const {
        my_destroy(ptr);
        my_delloc(ptr);
    }
};

} // namespace my

#endif // RICKY_MEMORY_HPP