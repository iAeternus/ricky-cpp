/**
 * @brief 可追踪内存泄漏的内存分配器
 * @note 由于需要追踪内存泄露，原则上不使用自定义的数据结构
 * @author Ricky
 * @date 2025/7/13
 * @version 1.0
 */
#ifndef TRACKING_ALLOCATOR_HPP
#define TRACKING_ALLOCATOR_HPP

#include "ricky.hpp"

#include <new>
#include <unordered_map>
#include <mutex>
#include <iostream>

namespace my::mem {

/**
 * @brief 可追踪内存泄漏的内存分配器
 * @tparam T 分配的对象类型
 * @details 记录每次分配和释放，程序结束时自动报告未释放的内存
 */
template <typename T>
class TrackingAllocator {
public:
    using value_type = T;
    using size_type = usize;
    using Self = TrackingAllocator<value_type>;

    using pointer = value_type*;
    using const_pointer = const value_type*;
    using void_pointer = void*;
    using const_void_pointer = const void*;
    using difference_type = std::ptrdiff_t;
    using is_always_equal = std::true_type;
    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_swap = std::true_type;

    template <typename U>
    struct rebind {
        using other = TrackingAllocator<U>;
    };

    TrackingAllocator() noexcept = default;
    template <typename U>
    TrackingAllocator(const TrackingAllocator<U>&) noexcept {}

    pointer allocate(size_type n) {
        if (n == 0) return nullptr;
        if (n > max_size()) throw std::bad_alloc();
        pointer p = static_cast<pointer>(::operator new(sizeof(value_type) * n));
        track_alloc(p, n);
        return p;
    }

    void deallocate(pointer p, size_type n) noexcept {
        if (p != nullptr) {
            track_dealloc(p);
            ::operator delete(p, n * sizeof(value_type));
        }
    }

    template <typename U, typename... Args>
    void construct(U* p, Args&&... args) {
        ::new (static_cast<void*>(p)) U(std::forward<Args>(args)...);
    }

    template <typename U>
    void destroy(U* p, size_type n = 1) {
        if (p == nullptr) return;
        for (size_type i = 0; i < n; ++i, ++p) {
            p->~U();
        }
    }

    template <typename... Args>
    pointer create(Args&&... args) {
        pointer ptr = allocate(1);
        if (!ptr) return nullptr;
        try {
            construct(ptr, std::forward<Args>(args)...);
        } catch (...) {
            destroy(ptr);
            deallocate(ptr, 1);
            throw;
        }
        return ptr;
    }

    static void report_leaks() {
        std::lock_guard<std::mutex> lock(leak_mutex_);
        if (alloc_map_.empty()) {
            std::cout << "[TrackingAllocator] No memory leaks detected." << std::endl;
        } else {
            std::cout << "[TrackingAllocator] Memory leaks detected:" << std::endl;
            for (const auto& [ptr, n] : alloc_map_) {
                std::cout << "  Leak at " << ptr << ", size: " << n << std::endl;
            }
        }
    }

private:
    /**
     * @brief 记录内存分配
     * @param p 分配的内存指针
     */
    static void track_alloc(void* p, size_type n) {
        std::lock_guard<std::mutex> lock(leak_mutex_);
        alloc_map_[p] = n;
    }

    /**
     * @brief 记录内存释放
     * @param p 释放的内存指针
     */
    static void track_dealloc(void* p) {
        std::lock_guard<std::mutex> lock(leak_mutex_);
        alloc_map_.erase(p);
    }

    /**
     * @brief 获取最大可分配内存大小
     * @return 最大可分配内存大小
     */
    static size_type max_size() noexcept {
        return size_type(-1) / sizeof(value_type);
    }

    static std::unordered_map<void*, size_type> alloc_map_; // 记录分配的内存和大小
    static std::mutex leak_mutex_;                          // 保护分配记录的互斥锁
};

template <typename T>
std::unordered_map<void*, usize> TrackingAllocator<T>::alloc_map_;

template <typename T>
std::mutex TrackingAllocator<T>::leak_mutex_;

// 分配器比较支持
template <typename T, typename U>
bool operator==(const TrackingAllocator<T>&, const TrackingAllocator<U>&) noexcept {
    return true;
}

template <typename T, typename U>
bool operator!=(const TrackingAllocator<T>&, const TrackingAllocator<U>&) noexcept {
    return false;
}

} // namespace my::mem

#endif // TRACKING_ALLOCATOR_HPP
