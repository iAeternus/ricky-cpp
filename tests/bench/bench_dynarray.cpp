#include "bench_dynarray.hpp"
#include "dyn_array.hpp"
#include "test/test_registry.hpp"

namespace my::bench::bench_dynarray {

static constexpr auto BENCH_CFG = BENCH_CONFIG(1, 1, 3);

constexpr i32 N = 1e6;

void speed_of_dny_array_append_string() {
    util::DynArray<std::string> d;
    for (usize i = 0; i < N; ++i) {
        d.append("aaaaa");
    }
}

void speed_of_std_vector_push_back_string() {
    std::vector<std::string> v;
    for (usize i = 0; i < N; ++i) {
        v.push_back("aaaaa");
    }
}

void speed_of_dny_array_append_i32() {
    util::DynArray<i32> d;
    for (usize i = 0; i < N; ++i) {
        d.append(i);
    }
}

void speed_of_std_vector_push_back_i32() {
    std::vector<i32> v;
    for (usize i = 0; i < N; ++i) {
        v.push_back(i);
    }
}

BENCH_NAME("bench_dynarray");
REGISTER_BENCH_TESTS(
    BENCH_TEST_ITEM_CFG(speed_of_dny_array_append_string, BENCH_CFG),
    BENCH_TEST_ITEM_CFG(speed_of_std_vector_push_back_string, BENCH_CFG),
    BENCH_TEST_ITEM_CFG(speed_of_dny_array_append_i32, BENCH_CFG),
    BENCH_TEST_ITEM_CFG(speed_of_std_vector_push_back_i32, BENCH_CFG))

} // namespace my::bench::bench_dynarray
