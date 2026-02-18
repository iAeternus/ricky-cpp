#include "bench_rbtree_map.hpp"

#include "random.hpp"
#include "rbtree_map.hpp"
#include "test_suite.hpp"
#include <map>

    namespace my::bench::bench_rbtree_map {

    static i32 g_n = 0;
    static util::Vec<i32> g_nums;

    static void setup_once() {
        if (g_n != 0) return;
        g_n = 1000000;
        g_nums.clear();
        g_nums.reserve(g_n);
        for (i32 i = 0; i < g_n; ++i) {
            g_nums.push(util::Random::instance().next<i32>(1, g_n));
        }
    }

    void test_sorted_hash_map_operations_speed() {
        setup_once();
        util::RBTreeMap<i32, i32> t;

        for (i32 i = 0; i < g_n; ++i) {
            t.insert(g_nums[i], 0);
        }

        for (i32 i = 0; i < g_n; ++i) {
            t[g_nums[i]]++;
        }

        for (i32 i = 0; i < g_n; ++i) {
            t.remove(g_nums[i]);
        }
    }

    void test_map_operations_speed() {
        setup_once();
        std::map<i32, i32> mp;

        for (i32 i = 0; i < g_n; ++i) {
            mp.emplace(g_nums[i], 0);
        }

        for (i32 i = 0; i < g_n; ++i) {
            mp[g_nums[i]]++;
        }

        for (i32 i = 0; i < g_n; ++i) {
            mp.erase(g_nums[i]);
        }
    }

    static constexpr auto BENCH_CFG = BENCH_CONFIG(1, 1, 3);
    BENCH_NAME("bench_rbtree_map");
    REGISTER_BENCH_TESTS(
        BENCH_TEST_ITEM_CFG(test_sorted_hash_map_operations_speed, BENCH_CFG),
        BENCH_TEST_ITEM_CFG(test_map_operations_speed, BENCH_CFG))

} // namespace my::bench::bench_rbtree_map
