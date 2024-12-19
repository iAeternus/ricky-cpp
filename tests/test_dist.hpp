#ifndef TEST_DIST_HPP
#define TEST_DIST_HPP

#include "ricky_test.hpp"
#include "Dict.hpp"

namespace my::test::test_dict {

auto should_insert = []() {
    // Given
    util::Dict<CString, int> d;

    // When
    d.insert(CString{"aaa"}, 1);
    d.insert(CString{"bbb"}, 3);
    d.insert(CString{"ccc"}, 2);

    // Then
    Assertions::assertEquals(3, int(d.size()));
    Assertions::assertTrue(d.contains(CString{"aaa"}));
    Assertions::assertFalse(d.contains(CString{"ddd"}));

    // When
    d.insert(CString{"ccc"}, 0);

    // Then
    Assertions::assertEquals(3, int(d.size()));
    Assertions::assertEquals(0, d.get(CString{"ccc"}));
};

auto should_get_or_default = []() {
    // Given
    util::Dict<int, int> d = {{1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}};

    // When
    int res1 = d.get(1);
    auto& res2 = d.getOrDefault(2, 0);
    auto& res3 = d.getOrDefault(6, 0);

    // Then
    Assertions::assertEquals(res1, 1);
    Assertions::assertEquals(res2, 1);
    Assertions::assertEquals(res3, 0);
};

auto should_set_default = []() {
    // Given
    util::Dict<int, int> d;

    // When
    d.setdefault(1, 1);

    // Then
    Assertions::assertEquals(1, d.get(1));

    // When
    d.setdefault(1, 2);

    // Then
    Assertions::assertEquals(1, d.get(1));
};

auto should_update = []() {
    // Given
    util::Dict<int, int> d = {{1, 1}, {2, 1}, {3, 1}};

    // When
    d.update({{4, 1}, {5, 1}});

    // Then
    Assertions::assertEquals(5, int(d.size()));
};

auto should_operator = []() {
    // Given
    util::Dict<int, int> d = {{1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}};
    util::Dict<int, int> d2 = {{4, 1}, {5, 1}, {6, 1}, {7, 1}, {8, 1}};

    // When
    auto res = d & d2;  // 交集
    auto res2 = d | d2; // 并集
    auto res3 = d ^ d2; // 相对补集
    auto res4 = d - d2; // 差集

    // Then
    Assertions::assertEquals(2, int(res.size()));
    Assertions::assertEquals(8, int(res2.size()));
    Assertions::assertEquals(6, int(res3.size()));
    Assertions::assertEquals(3, int(res4.size()));
};

void test_dict() {
    UnitTestGroup group{"test_dict"};

    group.addTest("should_insert", should_insert);
    group.addTest("should_get_or_default", should_get_or_default);
    group.addTest("should_set_default", should_set_default);
    group.addTest("should_update", should_update);
    group.addTest("should_operator", should_operator);

    group.startAll();
}

int n, k;
std::vector<int> nums;
std::vector<std::string> strs;

auto setup = []() {
    srand((unsigned) time(nullptr));
    n = 1e6;
    k = 100;
    for(int i = 0; i < n; ++i) {
        nums.push_back(rand() % k);
        strs.push_back(std::to_string(i));
    }
};

auto speed_of_dict_count = []() {
    util::Dict<int, int> d;
    for(const auto& num : nums) {
        ++d[num];
    }
};

auto speed_of_unordered_map_count = []() {
    std::unordered_map<int, int> mp;
    for(const auto& num : nums) {
        ++mp[num];
    }
};

auto speed_of_dict_insert = []() {
    util::Dict<std::string, int> d;
    for(int i = 0; i < n; ++i) {
        d.insert(strs[i], i);
    }
};

auto speed_of_unordered_map_insert = []() {
    std::unordered_map<std::string, int> mp;
    for(int i = 0; i < n; ++i) {
        mp.insert(std::make_pair(strs[i], i));
    }
};

void test_dict_speed() {
    UnitTestGroup group{"test_dict_speed"};
    group.setup(setup);

    group.addTest("speed_of_dict_count", speed_of_dict_count);
    group.addTest("speed_of_unordered_map_count", speed_of_unordered_map_count);
    group.addTest("speed_of_dict_insert", speed_of_dict_insert);
    group.addTest("speed_of_unordered_map_insert", speed_of_unordered_map_insert);

    group.startAll();
}

} // namespace my::test::test_dict

#endif // TEST_DIST_HPP