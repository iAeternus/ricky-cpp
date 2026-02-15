#include "bench_string_builder.hpp"

#include "test/test_registry.hpp"
#include "str_builder.hpp"

#include <string>

namespace my::bench::bench_string_builder {

static constexpr auto BENCH_CFG = BENCH_CONFIG(1, 1, 3);

constexpr usize N = 100000;

void speed_of_string_builder_append_string() {
    util::StringBuilder sb;
    for (usize i = 0; i < N; ++i) {
        sb.append("abcdef");
    }
    auto _ = sb.build();
}

void speed_of_std_string_append_string() {
    std::string str;
    for (usize i = 0; i < N; ++i) {
        str += "abcdef";
    }
}

BENCH_NAME("bench_string_builder");
REGISTER_BENCH_TESTS(
    BENCH_TEST_ITEM_CFG(speed_of_string_builder_append_string, BENCH_CFG),
    BENCH_TEST_ITEM_CFG(speed_of_std_string_append_string, BENCH_CFG))

} // namespace my::bench::bench_string_builder
