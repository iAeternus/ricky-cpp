#include "bench_queue.hpp"

#include "link_list_queue.hpp"
#include "random.hpp"
#include "test_suite.hpp"
#include <queue>

    namespace my::bench::bench_queue {

    constexpr i32 N = 1000000;

    void speed_of_util_queue_push_and_pop() {
        util::Queue<CString> q;
        for (usize i = 0; i < N; ++i) {
            q.push(util::Random::instance().next_str(3));
        }
        while (!q.empty()) {
            q.pop();
        }
    }

    void speed_of_std_queue_push_and_pop() {
        std::queue<CString> q;
        for (usize i = 0; i < N; ++i) {
            q.push(util::Random::instance().next_str(3));
        }
        while (!q.empty()) {
            q.pop();
        }
    }

    static constexpr auto BENCH_CFG = BENCH_CONFIG(1, 1, 3);
    BENCH_NAME("bench_queue");
    REGISTER_BENCH_TESTS(
        BENCH_TEST_ITEM_CFG(speed_of_util_queue_push_and_pop, BENCH_CFG),
        BENCH_TEST_ITEM_CFG(speed_of_std_queue_push_and_pop, BENCH_CFG))

} // namespace my::bench::bench_queue
