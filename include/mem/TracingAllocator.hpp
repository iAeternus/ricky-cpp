/**
 * @brief 可追踪内存泄漏的内存分配器
 * @note 由于需要追踪内存泄露，原则上不使用自定义的数据结构
 * @author Ricky
 * @date 2025/7/13
 * @version 2.0
 */
#ifndef TRACKING_ALLOCATOR_HPP
#define TRACKING_ALLOCATOR_HPP

#include "ricky.hpp"

#include <atomic>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <mutex>
#include <new>
#include <string>
#include <unordered_map>
#include <format>
#include <iostream>

namespace my::mem {

fn time_conv(time_t t) -> std::tm {
    std::tm tm;
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    return tm;
}

fn format_time(const std::tm& tm, const char* fmt) -> std::string {
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

    pointer allocate(size_type n) {
        if (n == 0) return nullptr;
        if (n > max_size()) throw std::bad_alloc();

        const usize size_bytes = sizeof(value_type) * n;
        pointer p = static_cast<pointer>(::operator new(size_bytes));

        const std::string stack = capture_stack();
        MemoryTracer::instance().trace_alloc(p, size_bytes, stack);
        return p;
    }

    void deallocate(pointer p, const size_type n) noexcept {
        if (p != nullptr) {
            MemoryTracer::instance().trace_dealloc(p);
            ::operator delete(p, n * sizeof(value_type));
        }
    }

    template <typename U, typename... Args>
    void construct(U* p, Args&&... args) {
        ::new (static_cast<void*>(p)) U(std::forward<Args>(args)...);
    }

    template <typename U>
    void destroy(U* p, const size_type n = 1) {
        if (p == nullptr) return;
        for (size_type i = 0; i < n; ++i, ++p) {
            p->~U();
        }
    }

    template <typename... Args>
    pointer create(Args&&... args) {
        auto* ptr = allocate(1);
        if (!ptr) return nullptr;

        try {
            construct(ptr, std::forward<Args>(args)...);
        } catch (...) {
            deallocate(ptr, 1);
            throw;
        }

        return ptr;
    }

    fn destruct(T* ptr) noexcept {
        if (ptr != nullptr) {
            destroy(ptr);
            deallocate(ptr, 1);
        }
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

    /**
     * @brief 获取最大可分配内存大小
     * @return 最大可分配内存大小
     */
    static size_type max_size() noexcept {
        return static_cast<size_type>(-1) / sizeof(value_type);
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