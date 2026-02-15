#include "bench_vec.hpp"

#include "test/test_registry.hpp"
#include "vec.hpp"

#include <string>
#include <vector>

namespace my::bench::bench_vec {

constexpr i32 N = 1000000;

void speed_of_util_vec_append_string() {
    util::Vec<std::string> d;
    for (usize i = 0; i < N; ++i) {
        d.push("aaaaa");
    }
}

void speed_of_std_vector_push_back_string() {
    std::vector<std::string> v;
    for (usize i = 0; i < N; ++i) {
        v.push_back("aaaaa");
    }
}

void speed_of_util_vec_append_i32() {
    util::Vec<i32> d;
    for (usize i = 0; i < N; ++i) {
        d.push(i);
    }
}

void speed_of_std_vector_push_back_i32() {
    std::vector<i32> v;
    for (usize i = 0; i < N; ++i) {
        v.push_back(i);
    }
}

static constexpr auto BENCH_CFG = BENCH_CONFIG(1, 1, 3);
BENCH_NAME("bench_vec");
REGISTER_BENCH_TESTS(
    BENCH_TEST_ITEM_CFG(speed_of_util_vec_append_string, BENCH_CFG),
    BENCH_TEST_ITEM_CFG(speed_of_std_vector_push_back_string, BENCH_CFG),
    BENCH_TEST_ITEM_CFG(speed_of_util_vec_append_i32, BENCH_CFG),
    BENCH_TEST_ITEM_CFG(speed_of_std_vector_push_back_i32, BENCH_CFG))

} // namespace my::bench::bench_vec
