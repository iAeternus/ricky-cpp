/**
 * @brief 内存分配器
 * @author Ricky
 * @date 2025/6/24
 * @version 1.0
 */
#ifndef ALLOCATOR_HPP
#define ALLOCATOR_HPP

#include "ricky.hpp"

#include <exception>

namespace my {

template <typename T>
class Allocator {
public:
    using Self = Allocator<T>;
    using value_type = T;
    using size_type = usize;
    
    using pointer = T*;
    using const_pointer = const T*;
    using void_pointer = void*;
    using const_void_pointer = const void*;
    using difference_type = std::ptrdiff_t;
    using is_always_equal = std::true_type;
    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_swap = std::true_type;

    Allocator() noexcept = default;

    template <typename U>
    Allocator(const Allocator<U>&) noexcept {}

    /**
     * @brief 分配连续n个T的内存，不会调用T的构造函数
     * @param size 分配多少个T的内存
     * @return 指向内存块首地址的指针
     */
    fn allocate(size_type n)->T* {
        if (n == 0) return nullptr;
        if (n > max_size()) throw std::bad_alloc();
        return static_cast<T*>(::operator new(sizeof(T) * n));
    }

    /**
     * @brief 释放p指向的内存
     * @param p 内存块首地址
     * @param n 内存块大小
     * @return void
     */
    fn deallocate(T* p, size_type n) noexcept {
        if(p != nullptr) {
            ::operator delete(p, n * sizeof(T));
        }
    }

    template <typename U, typename... Args>
    fn construct(U* p, Args&&... args)->void {
        ::new (static_cast<void*>(p)) U(std::forward<Args>(args)...);
    }

    template <typename U>
    fn destroy(U* p, size_type n = 1)->void {
        if (p == nullptr) return;
        for (size_type i = 0; i < n; ++i, ++p) {
            p->~U();
        }
    }

private:
    /**
     * @brief 能分配的最大内存大小
     */
    fn max_size() const noexcept {
        return size_type(-1) / sizeof(T);
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