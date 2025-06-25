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
    using difference_type = std::ptrdiff_t;
    using propagate_on_container_move_assignment = std::true_type;
    using is_always_equal = std::true_type;

    Allocator() noexcept = default;

    template <typename U>
    Allocator(const Allocator<U>&) noexcept {}

    /**
     * @brief 分配连续n个T的内存，不会调用T的构造函数
     * @param size 分配多少个T的内存
     * @return 指向这段连续空间首地址的指针
     */
    fn allocate(size_type n)->T* {
        if (n == 0) return nullptr;
        if (n > max_size()) throw std::bad_alloc();
        return static_cast<T*>(::operator new(sizeof(T) * n));
    }

    /**
     * @brief 释放p指向的内存
     * @param ptr 数组首地址
     * @return void
     */
    fn deallocate(T* p) noexcept {
        ::operator delete(p);
    }

    template <typename U, typename... Args>
    fn construct(U* p, Args&&... args)->void {
        // ::new (static_cast<void*>(p)) U(std::forward<Args>(args)...);
        ::new (p) U(std::forward<Args>(args)...);
    }

    template <typename U>
    fn destroy(U* p, size_type n = 1)->void {
        if (p == nullptr) return;
        for (size_type i = 0; i < n; ++i, ++p) {
            p->~T();
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