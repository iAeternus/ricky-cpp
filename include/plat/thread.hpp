#ifndef PLAT_THREAD_HPP
#define PLAT_THREAD_HPP

#include "my_types.hpp"

namespace my::plat::thread {

/**
 * @brief 不透明线程句柄
 */
struct ThreadHandle;

/**
 * @brief 创建线程
 */
ThreadHandle* create(void (*entry)(void*), void* arg);

/**
 * @brief 等待线程结束
 */
void join(ThreadHandle* thread);

/**
 * @brief 分离线程
 */
void detach(ThreadHandle* thread);

/**
 * @brief 是否可join
 */
bool joinable(ThreadHandle* thread);

/**
 * @brief 线程休眠
 */
void sleep_ms(u32 ms);

} // namespace my::plat::thread

#endif // PLAT_THREAD_HPP
