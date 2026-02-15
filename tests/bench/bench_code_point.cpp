#include "bench_code_point.hpp"

#include "code_point.hpp"
#include "test/test_registry.hpp"

namespace my::bench::bench_code_point {

static constexpr auto BENCH_CFG = BENCH_CONFIG(1, 10, 3);
constexpr usize N = 1000000;

void speed_of_code_point_compare() {
    util::CodePoint cp{'a'};
    volatile usize hit = 0;
    for (usize i = 0; i < N; ++i) {
        if (cp == 'a') {
            ++hit;
        }
    }
    (void)hit;
}

BENCH_NAME("bench_code_point");
REGISTER_BENCH_TESTS(
    BENCH_TEST_ITEM_CFG(speed_of_code_point_compare, BENCH_CFG))

} // namespace my::bench::bench_code_point
