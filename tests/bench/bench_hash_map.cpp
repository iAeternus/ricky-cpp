#include "bench_hash_map.hpp"

#include "hash_map.hpp"
#include "random.hpp"
#include "test_suite.hpp"

#include <string>
#include <unordered_map>
#include <vector>

namespace my::bench::bench_hash_map {

static usize g_n = 0;
static i32 g_k = 0;
static std::vector<i32> g_nums;
static std::vector<std::string> g_strs;

static void setup_once() {
    if (g_n != 0) return;
    g_n = 1000000;
    g_k = 100;
    g_nums.reserve(g_n);
    g_strs.reserve(g_n);
    for (usize i = 0; i < g_n; ++i) {
        g_nums.push_back(util::Random::instance().next<i32>(0, g_k));
        g_strs.push_back(std::to_string(i));
    }
}

void speed_of_hash_map_count() {
    setup_once();
    util::HashMap<i32, i32> d;
    for (const auto& num : g_nums) {
        ++d[num];
    }
}

void speed_of_unordered_map_count() {
    setup_once();
    std::unordered_map<i32, i32> mp;
    for (const auto& num : g_nums) {
        ++mp[num];
    }
}

void speed_of_hash_map_insert() {
    setup_once();
    util::HashMap<std::string, i32> d;
    for (usize i = 0; i < g_n; ++i) {
        d.insert(g_strs[i], 1);
    }
}

void speed_of_unordered_map_insert() {
    setup_once();
    std::unordered_map<std::string, i32> mp;
    for (usize i = 0; i < g_n; ++i) {
        mp.insert(std::make_pair(g_strs[i], 1));
    }
}

static constexpr auto BENCH_CFG = BENCH_CONFIG(1, 1, 3);
BENCH_NAME("bench_hash_map");
REGISTER_BENCH_TESTS(
    BENCH_TEST_ITEM_CFG(speed_of_hash_map_count, BENCH_CFG),
    BENCH_TEST_ITEM_CFG(speed_of_unordered_map_count, BENCH_CFG),
    BENCH_TEST_ITEM_CFG(speed_of_hash_map_insert, BENCH_CFG),
    BENCH_TEST_ITEM_CFG(speed_of_unordered_map_insert, BENCH_CFG))

} // namespace my::bench::bench_hash_map
