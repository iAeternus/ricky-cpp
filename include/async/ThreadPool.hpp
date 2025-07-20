/**
 * @brief 线程池
 * @author Ricky
 * @date 2025/1/1
 * @version 1.0
 */
#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include "Array.hpp"
#include "NoCopy.hpp"
#include "Queue.hpp"

#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>

namespace my::async {

/**
 * @brief 停止标记
 */
struct StopFlag {
    static constexpr i8 WAIT_FOREVER = -1; // 不停止
    static constexpr i8 STOP_NOW = 0;      // 立即停止
    static constexpr i8 STOP_FINISHED = 1; // 等待所有任务完成后停止
};

/**
 * @brief 线程池
 */
class ThreadPool : public Object<ThreadPool>, public NoCopy {
    using Self = ThreadPool;

public:
    using Task = Runnable;

    ThreadPool(usize num_of_threads) :
            threads_(num_of_threads), stop_flag_(StopFlag::WAIT_FOREVER) {
        auto worker = [this]() {
            loop {
                std::unique_lock<std::mutex> lock(mtx_);
                condition_.wait(lock, [this]() {
                    return is_stop_now() || is_task_finished() || has_task();
                });

                if (is_stop_now() || is_task_finished()) {
                    return;
                }

                Task task = std::move(tasks_.front());
                tasks_.pop();

                lock.unlock();
                task();
            }
        };
        for (auto& thread : threads_) {
            thread = std::thread(worker);
        }
    }

    ~ThreadPool() {
        stop();
    }

    template <typename F, typename... Args>
    auto push(F&& task, Args&&... args) -> std::future<decltype(task(args...))> {
        using return_type = decltype(task(args...));

        auto task_ptr = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(task), std::forward<Args>(args)...));
        std::future<return_type> res = task_ptr->get_future();

        {
            std::lock_guard<std::mutex> lock(mtx_);
            tasks_.push([task_ptr]() { (*task_ptr)(); });
        }

        condition_.notify_one();
        return res;
    }

    /**
     * @brief 立即停止线程池
     */
    void stop() {
        set_stop_flag(StopFlag::STOP_NOW);
        join();
    }

    /**
     * @brief 等待所有任务完成后停止线程池
     */
    void wait() {
        set_stop_flag(StopFlag::STOP_FINISHED);
        join();
    }

private:
    void join() {
        for (auto&& thread : threads_) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    void set_stop_flag(i8 flag) {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            stop_flag_ = flag;
        }
        condition_.notify_all();
    }

    /**
     * @brief 判断是否立即停止，默认此时持有锁
     * @return true=是 false=否
     */
    bool is_stop_now() const {
        return stop_flag_ == StopFlag::STOP_NOW;
    }

    /**
     * @brief 判断任务是否完成
     * @return true=是 false=否
     */
    bool is_task_finished() const {
        return stop_flag_ == StopFlag::STOP_FINISHED && tasks_.empty();
    }

    /**
     * @brief 判断任务队列中是否有任务
     * @return true=是 false=否
     */
    bool has_task() const {
        return !tasks_.empty();
    }

private:
    util::Array<std::thread> threads_;
    util::Queue<Task> tasks_;
    std::mutex mtx_;
    std::condition_variable condition_;
    i8 stop_flag_;
};

} // namespace my::async

#endif // THREAD_POOL_HPP