/**
 * @brief 内存分配器
 * @author Ricky
 * @date 2025/12/6
 * @version 1.0
 */

#ifndef ALLOC_HPP
#define ALLOC_HPP

#include <cstddef>
#include <new>
#include <memory>
#include <type_traits>
#include <bit>

namespace my::mem {

template <typename Ptr>
struct AllocationResult {
    Ptr ptr;
    std::size_t count;
};

/**
 * @class Allocator
 * @tparam T 分配的元素类型
 */
template <typename T>
class Allocator {
public:
    using Self = Allocator<T>;
    using value_type = T;

    using is_always_equal = std::true_type;
    using propagate_on_container_copy_assignment = std::true_type;
    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_swap = std::true_type;

    constexpr Allocator() noexcept = default;

    template<typename U>
    constexpr Allocator(const Allocator<U>&) noexcept {}

    template <typename U>
    constexpr Allocator(const Self&) noexcept {}

    template <typename U>
    struct rebind {
        using other = Allocator<U>;
    };

    /**
     * @brief 分配指定数量的内存
     * @note 分配足够容纳n个T类型对象的内存块。当请求大小超过最大允许值时抛出bad_alloc异常
     * @note 自动处理超过默认对齐要求的对齐分配
     * @param n 需要分配的元素数量
     * @return 指向分配内存起始地址的指针
     * @throw std::bad_alloc 当内存分配失败或请求大小超过max_size()时抛出
     */
    [[nodiscard]] auto allocate(std::size_t n) -> T* {
        if (n == 0) return nullptr;
        if (n > max_size()) [[unlikely]] {
            throw std::bad_alloc();
        }

        std::size_t bytes = n * sizeof(T);

        // 对齐
        if constexpr (alignof(T) > __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
            return static_cast<T*>(::operator new(bytes, std::align_val_t(alignof(T))));
        }
        return static_cast<T*>(::operator new(bytes));
    }

    /**
     * @brief 释放先前分配的内存
     * @note 释放由allocate分配的内存块。对空指针调用是安全的
     * @note 自动处理对齐释放以匹配分配时的对齐方式
     * @param p 要释放的内存指针
     * @param n 先前分配的元素数量，用于大小计算
     */
    auto deallocate(T* p, std::size_t n) noexcept -> void {
        if (!p) return;

        std::size_t bytes = n * sizeof(T);
        if constexpr (alignof(T) > __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
            ::operator delete(p, bytes, std::align_val_t(alignof(T)));
        } else {
            ::operator delete(p, bytes);
        }
    }

    /**
     * @brief 超额分配内存
     * @note 分配至少能容纳n个元素的内存，实际分配数量可能更多（向上取2的幂）。
     * @note 返回实际分配的元素数量，可用于优化容器性能。
     * @param n 请求的最小元素数量
     * @return 包含分配指针和实际元素数量的结构体
     * @throw std::bad_alloc 当内存分配失败时抛出
     */
    [[nodiscard]] auto allocate_at_least(std::size_t n) -> AllocationResult<T*> {
        if (n == 0) return {nullptr, 0};
        // 向上取 2 的幂
        std::size_t count = std::bit_ceil(n);
        return {allocate(count), count};
    }

    /**
     * @brief 对齐分配内存
     * @note 分配具有指定对齐要求的内存。对齐值必须是2的幂且不小于alignof(T)。
     * @note 当对齐要求不超过默认对齐时，回退到普通分配。
     * @tparam Alignment 要求的对齐字节数，默认为alignof(T)
     * @param n 需要分配的元素数量
     * @return 对齐分配的内存指针
     * @throw std::bad_alloc 当内存分配失败时抛出
     */
    template <std::size_t Alignment/* = alignof(T)*/>
    [[nodiscard]] auto allocate_aligned(std::size_t n) -> T* {
        static_assert((Alignment & (Alignment - 1)) == 0, "Alignment must be power of two");
        static_assert(Alignment >= alignof(T), "Alignment must be at least alignof(T)");

        std::size_t bytes = n * sizeof(T);
        if constexpr (Alignment <= __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
            return allocate(n); // 直接走 normal allocate
        }
        return static_cast<T*>(::operator new(bytes, std::align_val_t(Alignment)));
    }

    /**
     * @brief 在已分配内存上构造单个对象
     * @note 使用完美转发参数在指定位置构造对象
     * @tparam U 构造的对象类型，通常为T或派生类型
     * @tparam Args 构造函数参数类型
     * @param p 构造对象的地址
     * @param args 传递给构造函数的参数
     */
    template <typename U, typename... Args>
    auto construct(U* p, Args&&... args) -> void {
        std::construct_at(p, std::forward<Args>(args)...);
    }

    /**
     * @brief 批量构造对象
     * @note 在连续内存区域构造多个对象。如果构造过程中抛出异常，
     * @note 已成功构造的对象会被正确销毁，异常继续传播。
     * @tparam U 构造的对象类型
     * @tparam Args 构造函数参数类型
     * @param p 构造起始地址
     * @param n 构造对象数量
     * @param args 传递给每个对象构造函数的参数
     * @throw 任意构造函数可能抛出的异常
     */
    template <typename U, typename... Args>
    auto construct_n(U* p, std::size_t n, Args&&... args) -> void {
        std::size_t constructed = 0;
        try {
            for (; constructed < n; ++constructed) {
                std::construct_at(p + constructed, std::forward<Args>(args)...);
            }
        } catch (...) {
            // 销毁已成功构造的部分
            if constexpr (!std::is_trivially_destructible_v<U>) {
                for (std::size_t i = 0; i < constructed; ++i) {
                    std::destroy_at(p + i);
                }
            }
            throw; // 重新抛出，让调用方决定后续，例如释放内存
        }
    }

    /**
     * @brief 销毁单个对象
     * @note 调用对象的析构函数。对平凡可析构类型进行编译时优化
     * @tparam U 销毁的对象类型
     * @param p 要销毁对象的地址
     */
    template <typename U>
    auto destroy(U* p) noexcept -> void {
        if constexpr (!std::is_trivially_destructible_v<U>) {
            std::destroy_at(p);
        }
    }

    /**
     * @brief 批量销毁对象
     * @note 销毁连续内存区域的多个对象。对平凡可析构类型进行编译时优化
     * @tparam U 销毁的对象类型
     * @param p 销毁起始地址
     * @param n 销毁对象数量
     */
    template <typename U>
    auto destroy_n(U* p, std::size_t n) noexcept -> void {
        if constexpr (!std::is_trivially_destructible_v<U>) {
            std::destroy_n(p, n);
        }
    }

    /**
     * @brief 安全创建单个对象
     * @note 分配内存并构造单个对象的便捷方法。如果构造失败，已分配的内存会被自动释放
     * @tparam Args 构造函数参数类型
     * @param args 传递给构造函数的参数
     * @return 成功时返回对象指针，失败时返回nullptr
     */
    template <typename... Args>
    [[nodiscard]] auto create(Args&&... args) noexcept -> T* {
        T* p = nullptr;
        try {
            p = allocate(1);
            construct(p, std::forward<Args>(args)...);
        } catch (...) {
            if (p) deallocate(p, 1);
            return nullptr;
        }
        return p;
    }

    /**
     * @brief 安全创建对象数组
     * @note 分配内存并构造多个相同对象的便捷方法。如果构造过程中抛出异常，
     * @note 已分配的内存会被自动释放，确保没有资源泄漏。
     * @tparam Args 构造函数参数类型
     * @param n 数组元素数量
     * @param args 传递给每个元素构造函数的参数
     * @return 成功时返回数组指针，失败时返回nullptr
     */
    template <typename... Args>
    [[nodiscard]] auto create_array(std::size_t n, Args&&... args) noexcept -> T* {
        if (n == 0) return nullptr;

        T* p = nullptr;
        try {
            p = allocate(n);
            // construct_n 内部保证：如果抛出，它已清理已构造元素，然后 rethrow
            construct_n(p, n, std::forward<Args>(args)...);
        } catch (...) {
            if (p) {
                // 只有当 construct_n 已成功构造全部元素（没抛）时，才需要 destroy_n
                // 但若 construct_n 抛出，则它已经清理了已构造的元素
                // 所以这里直接 deallocate 即可
                deallocate(p, n);
            }
            return nullptr;
        }
        return p;
    }

    /**
     * @brief 获取最大可分配元素数量
     * @note 计算理论上可分配的最大元素数量，基于类型大小和地址空间限制
     * @return 最大可分配元素数量
     */
    static constexpr auto max_size() noexcept -> std::size_t {
        return static_cast<std::size_t>(-1) / sizeof(T);
    }
};

/**
 * @brief 分配器相等比较运算符
 * @note 所有Allocator特化实例都被视为相等，支持任意类型特化之间的互操作
 * @tparam T 左侧分配器的元素类型
 * @tparam U 右侧分配器的元素类型
 * @return true 总是返回true
 */
template <typename T, typename U>
constexpr auto operator==(const Allocator<T>&, const Allocator<U>&) noexcept -> bool {
    return true;
}

} // namespace my::mem

#endif // ALLOC_HPP
