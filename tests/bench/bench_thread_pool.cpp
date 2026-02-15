#include "bench_thread_pool.hpp"

#include "test/test_registry.hpp"
#include "thread_pool.hpp"

#include <chrono>
#include <thread>

namespace my::bench::bench_thread_pool {

static constexpr auto BENCH_CFG = BENCH_CONFIG(1, 1, 3);
static usize g_n = 50;

static void task() {
    std::this_thread::sleep_for(std::chrono::milliseconds(3));
}

void speed_of_thread_pool() {
    async::ThreadPool tp{100};
    for (usize i = 0; i < g_n; ++i) {
        tp.push(task);
    }
    tp.wait();
}

void speed_of_sync() {
    for (usize i = 0; i < g_n; ++i) {
        task();
    }
}

BENCH_NAME("bench_thread_pool");
REGISTER_BENCH_TESTS(
    BENCH_TEST_ITEM_CFG(speed_of_thread_pool, BENCH_CFG),
    BENCH_TEST_ITEM_CFG(speed_of_sync, BENCH_CFG))

} // namespace my::bench::bench_thread_pool
