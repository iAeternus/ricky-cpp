/**
 * @brief 内存分配器
 * @author Ricky
 * @date 2025/6/24
 * @version 1.0
 */
#ifndef ALLOCATOR_HPP
#define ALLOCATOR_HPP

#include "ricky.hpp"

#include <new>

namespace my {

/**
 * @brief 分配器类，提供内存分配、释放和对象构造、析构的功能
 * @tparam T 分配的对象类型
 * @details 该分配器使用全局的 new 和 delete 操作符进行内存管理，支持对象的构造和析构。
 *          适用于需要自定义内存管理的场景，如 STL 容器的自定义分配器。
 *          注意：该分配器不支持线程安全，使用时需确保线程安全性。
 */
template <typename T>
class Allocator {
public:
    using value_type = T;
    using size_type = usize;
    using Self = Allocator<value_type>;

    using pointer = value_type*;
    using const_pointer = const value_type*;
    using void_pointer = void*;
    using const_void_pointer = const void*;
    using difference_type = std::ptrdiff_t;
    using is_always_equal = std::true_type;
    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_swap = std::true_type;

    /**
     * @brief 默认构造函数
     * @details 不执行任何操作，适用于默认初始化分配器
     */
    Allocator() noexcept = default;

    /**
     * @brief 拷贝构造函数
     * @tparam U 其他类型的分配器
     * @param other 其他类型的分配器实例
     * @details 允许从其他类型的分配器转换而来，适用于 STL 容器的 rebind 操作
     */
    template <typename U>
    Allocator(const Allocator<U>&) noexcept {}

    /**
     * @brief 分配内存
     * @details 分配n个value_type类型对象的内存，不会调用对象的构造函数
     *          如果n为0，则返回nullptr；如果n超过最大可分配大小，则抛出 std::bad_alloc 异常
     *          使用全局的 new 操作符进行内存分配
     * @param n 要分配的对象数量
     * @return 指向分配内存的指针，若分配失败则抛出 std::bad_alloc 异常
     */
    fn allocate(size_type n)->pointer {
        if (n == 0) return nullptr;
        if (n > max_size()) throw std::bad_alloc();
        return static_cast<pointer>(::operator new(sizeof(value_type) * n));
    }

    /**
     * @brief 释放内存
     * @details 释放p指向的内存块，n为要释放的对象数量
     *          如果p为nullptr，则不执行任何操作
     *          使用全局的 delete 操作符进行内存释放
     * @param p 指向要释放内存的指针
     * @param n 要释放的对象数量
     */
    fn deallocate(pointer p, size_type n) noexcept {
        if (p != nullptr) {
            ::operator delete(p, n * sizeof(value_type));
        }
    }

    /**
     * @brief 构造对象
     * @details 在p指向的内存位置构造一个value_type类型的对象，使用给定的参数
     *          如果p为nullptr，则不执行任何操作
     *          使用placement new进行对象构造
     * @tparam U 要构造的对象类型
     * @param p 指向要构造对象的内存位置
     * @param args 构造函数的参数
     */
    template <typename U, typename... Args>
    fn construct(U* p, Args&&... args)->void {
        ::new (static_cast<void*>(p)) U(std::forward<Args>(args)...);
    }

    /**
     * @brief 析构对象
     * @details 在p指向的内存位置析构一个value_type类型的对象，n为要析构的对象数量
     *          如果p为nullptr，则不执行任何操作
     *          使用对象的析构函数进行析构
     * @tparam U 要析构的对象类型
     * @param p 指向要析构对象的内存位置
     * @param n 要析构的对象数量
     */
    template <typename U>
    fn destroy(U* p, size_type n = 1)->void {
        if (p == nullptr) return;
        for (size_type i = 0; i < n; ++i, ++p) {
            p->~U();
        }
    }

    /**
     * @brief 安全创建对象，返回构造好的指针
     * @tparam Args 构造函数的参数类型
     * @param args 构造函数的参数
     * @return 指向新对象的指针，若失败返回 nullptr
     * @details 在分配的内存中构造一个value_type类型的对象，使用给定的参数
     *          如果分配失败，则返回nullptr，并且在异常情况下会自动释放内存
     */
    template <typename... Args>
    fn create(Args&&... args)->pointer {
        auto* ptr = allocate(1);
        if (!ptr) return nullptr;

        try {
            construct(ptr, std::forward<Args>(args)...);
        } catch (...) {
            destroy(ptr);
            throw;
        }

        return ptr;
    }

    // /**
    //  * @brief 安全析构对象并释放内存
    //  * @param ptr 指向对象的指针
    //  */
    // fn destruct(T* ptr) const noexcept {
    //     if (ptr != nullptr) {
    //         destroy(ptr);
    //         deallocate(ptr, 1);
    //     }
    // }

private:
    /**
     * @brief 获取最大可分配大小
     * @details 返回当前分配器可以分配的最大对象数量
     *          计算方式为 size_type(-1) / sizeof(value_type)
     * @return 最大可分配对象数量
     */
    fn max_size() const noexcept {
        return size_type(-1) / sizeof(value_type);
    }
};

// 分配器比较支持
template <typename U>
struct rebind {
    using other = Allocator<U>;
};

template <typename T, typename U>
bool operator==(const Allocator<T>&, const Allocator<U>&) noexcept {
    return true;
}

template <typename T, typename U>
bool operator!=(const Allocator<T>&, const Allocator<U>&) noexcept {
    return false;
}

} // namespace my

#endif // ALLOCATOR_HPP