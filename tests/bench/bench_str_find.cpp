#include "bench_str_find.hpp"

#include "test_suite.hpp"
#include "str/string.hpp"

#include <cstring>
#include <string>

namespace my::bench::bench_str_find {

namespace {

constexpr usize N = 200;

std::string make_hay() {
    std::string s;
    s.reserve(200000);
    for (usize i = 0; i < 8000; ++i) {
        s += "abcdefghijklmnopqrstuvwxyz";
    }
    return s;
}

Option<usize> naive_find(const my::str::StringView& hay, const my::str::StringView& pat) {
    if (pat.len() == 0) return Option<usize>::Some(0);
    if (pat.len() > hay.len()) return Option<usize>::None();
    const u8* h = hay.as_bytes();
    const u8* p = pat.as_bytes();
    for (usize i = 0; i + pat.len() <= hay.len(); ++i) {
        if (std::memcmp(h + i, p, pat.len()) == 0) {
            return Option<usize>::Some(i);
        }
    }
    return Option<usize>::None();
}

} // namespace

void speed_of_str_find_twoway() {
    static const std::string hay = make_hay();
    static const my::str::String hay_s(hay.c_str(), hay.size());
    static const my::str::StringView pat("mnopqrstuv");

    for (usize i = 0; i < N; ++i) {
        auto pos = hay_s.as_str().find(pat);
        (void)pos;
    }
}

void speed_of_str_find_naive() {
    static const std::string hay = make_hay();
    static const my::str::StringView hay_v(hay.c_str(), hay.size());
    static const my::str::StringView pat("mnopqrstuv");

    for (usize i = 0; i < N; ++i) {
        auto pos = naive_find(hay_v, pat);
        (void)pos;
    }
}

static constexpr auto BENCH_CFG = BENCH_CONFIG(1, 1, 3);
BENCH_NAME("bench_str_find");
REGISTER_BENCH_TESTS(
    BENCH_TEST_ITEM_CFG(speed_of_str_find_twoway, BENCH_CFG),
    BENCH_TEST_ITEM_CFG(speed_of_str_find_naive, BENCH_CFG))

} // namespace my::bench::bench_str_find
