/**
 * @brief 可追踪内存泄漏的内存分配器
 * @note 由于需要追踪内存泄露，原则上不使用自定义的数据结构
 * @author Ricky
 * @date 2025/7/13
 * @version 2.0
 */
#ifndef TRACKING_ALLOCATOR_HPP
#define TRACKING_ALLOCATOR_HPP

#include "alloc.hpp"
#include "my_config.hpp"
#include "my_types.hpp"

#include <atomic>
#include <chrono>
#include <ctime>
#include <new>
#include <string>
#include <unordered_map>
#include <format>
#include <iostream>

namespace my::mem {

inline auto time_conv(time_t t) -> std::tm {
    std::tm tm;
#if RICKY_WIN
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif // RICKY_WIN
    return tm;
}

inline auto format_time(const std::tm& tm, const char* fmt) -> std::string {
    char buffer[80];
    if (std::strftime(buffer, sizeof(buffer), fmt, &tm)) {
        return buffer;
    }
    return "time_format_error";
}

/**
 * @brief 分配信息记录
 */
struct AllocationRecord {
    usize id;                                   // 分配的唯一ID
    usize size;                                 // 分配的字节数
    std::chrono::system_clock::time_point time; // 分配时间
    std::string stack;                          // 调用栈信息
};

/**
 * @brief 内存追踪器，单例类
 */
class MemoryTracer {
public:
    MemoryTracer() {
        // 注册退出时报告函数
        std::atexit([]() {
            instance().report_on_exit();
        });
    }

    static MemoryTracer& instance() {
        static MemoryTracer tracker;
        return tracker;
    }

    /**
     * @brief 追踪内存申请
     */
    void trace_alloc(void* ptr, const usize size, const std::string& stack = "") {
        // std::lock_guard lock(mutex_);

        // 生成分配记录
        const auto now = std::chrono::system_clock::now();
        const AllocationRecord record{++alloc_count_, size, now, stack};

        // 更新内存统计
        current_memory_ += size;
        peak_memory_.store(std::max(peak_memory_.load(), current_memory_.load()));
        total_allocated_ += size;

        // 存储记录
        alloc_map_[ptr] = record;

        // 输出详细日志
        if (verbose_) {
            const auto t = std::chrono::system_clock::to_time_t(now);
            const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

            auto time_str = format_time(time_conv(t), "%T");
            std::cout << std::format(
                "[ALLOC] {} ID: {} Size: {} bytes Time: {}.{:03d} Current: {} bytes Peak: {} bytes\n",
                ptr,
                record.id,
                size,
                time_str,
                static_cast<int>(ms.count()),
                current_memory_.load(),
                peak_memory_.load());
        }
    }

    /**
     * @brief 追踪内存释放
     */
    void trace_dealloc(void* ptr) {
        // std::lock_guard lock(mutex_);

        const auto it = alloc_map_.find(ptr);
        if (it == alloc_map_.end()) {
            if (verbose_) {
                std::cout << "[DEALLOC] " << ptr << " not found!\n";
            }
            return;
        }

        // 更新内存统计
        const usize size = it->second.size;
        current_memory_ -= size;
        total_deallocated_ += size;

        // 输出详细日志
        if (verbose_) {
            const auto now = std::chrono::system_clock::now();
            const auto t = std::chrono::system_clock::to_time_t(now);
            const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

            auto time_str = format_time(time_conv(t), "%T");
            std::cout << std::format(
                "[FREE] {} ID: {} Size: {} bytes Time: {}.{:03d} Current: {} bytes\n",
                ptr,
                it->second.id,
                size,
                time_str,
                static_cast<int>(ms.count()),
                current_memory_.load());
        }

        // 移除记录
        alloc_map_.erase(it);
    }

    /**
     * @brief 报告内存泄露
     */
    void report_leaks() const {
        // std::lock_guard lock(mutex_);

        if (alloc_map_.empty()) {
            std::cout << "\n[TRACKER] No memory leaks detected\n";
        } else {
            std::cout << "\n[TRACKER] Memory leaks detected: "
                      << alloc_map_.size() << " allocations\n";

            for (const auto& [ptr, record] : alloc_map_) {
                const auto t = std::chrono::system_clock::to_time_t(record.time);
                auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(record.time.time_since_epoch()) % 1000;

                auto time_str = format_time(time_conv(t), "%T");
                std::cout << std::format(
                    "  Leak #{} at {} - Size: {} bytes Allocated at: {}.{:03d}\n",
                    record.id,
                    ptr,
                    record.size,
                    time_str,
                    static_cast<int>(ms.count()));
            }
        }

        // 打印内存统计摘要
        std::cout << std::format(
            "\n[TRACKER] Memory usage summary:\n"
            "  Current memory:   {} bytes\n"
            "  Peak memory:      {} bytes\n"
            "  Total allocated:  {} bytes\n"
            "  Total deallocated:{} bytes\n"
            "  Net memory:       {} bytes\n",
            current_memory_.load(),
            peak_memory_.load(),
            total_allocated_.load(),
            total_deallocated_.load(),
            total_allocated_.load() - total_deallocated_.load());
    }

    void set_verbose(const bool verbose) noexcept {
        verbose_ = verbose;
    }

private:
    void report_on_exit() const {
        if (!reported_) {
            report_leaks();
            reported_ = true;
        }
    }

private:
    // mutable std::mutex mutex_;
    std::unordered_map<void*, AllocationRecord> alloc_map_;

    std::atomic<usize> current_memory_{0};    // 当前内存使用量
    std::atomic<usize> peak_memory_{0};       // 峰值内存使用量
    std::atomic<usize> total_allocated_{0};   // 总分配内存量
    std::atomic<usize> total_deallocated_{0}; // 总释放内存量
    std::atomic<usize> alloc_count_{0};       // 分配操作计数器

    std::atomic<bool> verbose_{false};          // 详细模式
    mutable std::atomic<bool> reported_{false}; // 是否已报告
};

/**
 * @brief 可追踪内存泄漏的内存分配器
 * @tparam T 分配的对象类型
 * @details 记录每次分配和释放，程序结束时自动报告未释放的内存
 */
template <typename T>
class TracingAllocator {
public:
    using value_type = T;
    using size_type = usize;
    using Self = TracingAllocator<value_type>;

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
        using other = TracingAllocator<U>;
    };

    TracingAllocator() noexcept = default;

    template <typename U>
    explicit TracingAllocator(const TracingAllocator<U>&) noexcept {}

    [[nodiscard]] auto allocate(std::size_t n) -> pointer {
        if (n == 0) return nullptr;
        if (n > max_size()) [[unlikely]] {
            throw std::bad_alloc();
        }

        std::size_t bytes = n * sizeof(T);
        pointer p = nullptr;
        if constexpr (alignof(T) > __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
            p = static_cast<pointer>(::operator new(bytes, std::align_val_t(alignof(T))));
        } else {
            p = static_cast<pointer>(::operator new(bytes));
        }
        const std::string stack = capture_stack();
        MemoryTracer::instance().trace_alloc(p, bytes, stack);
        return p;
    }

    auto deallocate(T* p, std::size_t n) noexcept -> void {
        if (!p) return;

        std::size_t bytes = n * sizeof(T);
        if constexpr (alignof(T) > __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
            MemoryTracer::instance().trace_dealloc(p);
            ::operator delete(p, bytes, std::align_val_t(alignof(T)));
        } else {
            MemoryTracer::instance().trace_dealloc(p);
            ::operator delete(p, bytes);
        }
    }

    [[nodiscard]] auto allocate_at_least(std::size_t n) -> AllocationResult<T*> {
        if (n == 0) return {nullptr, 0};
        // 向上取 2 的幂
        std::size_t count = std::bit_ceil(n);
        return {allocate(count), count};
    }

    // TODO allocate_aligned

    template <typename U, typename... Args>
    auto construct(U* p, Args&&... args) -> void {
        std::construct_at(p, std::forward<Args>(args)...);
    }

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

    template <typename U>
    auto destroy(U* p) noexcept -> void {
        if constexpr (!std::is_trivially_destructible_v<U>) {
            std::destroy_at(p);
        }
    }

    template <class U>
    auto destroy_n(U* p, std::size_t n) noexcept -> void {
        if constexpr (!std::is_trivially_destructible_v<U>) {
            std::destroy_n(p, n);
        }
    }

    template <class... Args>
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

    template <class... Args>
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

    static constexpr auto max_size() noexcept -> std::size_t {
        return static_cast<std::size_t>(-1) / sizeof(T);
    }

    /**
     * @brief 设置详细日志模式
     * @param verbose
     */
    static void set_verbose(const bool verbose) noexcept {
        MemoryTracer::instance().set_verbose(verbose);
    }

    /**
     * @brief 手动报告内存泄漏
     */
    static void report_leaks() {
        MemoryTracer::instance().report_leaks();
    }

private:
    static std::string capture_stack() {
        return "Stack capture not implemented";
    }
};

// 分配器比较支持
template <typename T, typename U>
bool operator==(const TracingAllocator<T>&, const TracingAllocator<U>&) noexcept {
    return true;
}

template <typename T, typename U>
bool operator!=(const TracingAllocator<T>&, const TracingAllocator<U>&) noexcept {
    return false;
}

} // namespace my::mem

#endif // TRACKING_ALLOCATOR_HPP
