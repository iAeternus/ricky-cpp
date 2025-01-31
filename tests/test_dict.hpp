#ifndef TEST_DICT_HPP
#define TEST_DICT_HPP

#include "ricky_test.hpp"
#include "Dict.hpp"
#include "Random.hpp"

namespace my::test::test_dict {

auto should_insert = []() {
    // Given
    util::Dict<CString, i32> d;

    // When
    d.insert("aaa"_cs, 1);
    d.insert("bbb"_cs, 3);
    d.insert("ccc"_cs, 2);

    // Then
    Assertions::assertEquals(3, i32(d.size()));
    Assertions::assertTrue(d.contains("aaa"_cs));
    Assertions::assertFalse(d.contains("ddd"_cs));

    // When
    d.insert("ccc"_cs, 0);

    // Then
    Assertions::assertEquals(3, i32(d.size()));
    Assertions::assertEquals(0, d.get("ccc"_cs));
};

auto should_get_or_default = []() {
    // Given
    util::Dict<i32, i32> d = {{1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}};

    // When
    i32 res1 = d.get(1);
    auto& res2 = d.getOrDefault(2, 0);
    auto& res3 = d.getOrDefault(6, 0);

    // Then
    Assertions::assertEquals(res1, 1);
    Assertions::assertEquals(res2, 1);
    Assertions::assertEquals(res3, 0);
};

auto should_set_default = []() {
    // Given
    util::Dict<i32, i32> d;

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
    util::Dict<i32, i32> d = {{1, 1}, {2, 1}, {3, 1}};

    // When
    d.update({{4, 1}, {5, 1}});

    // Then
    Assertions::assertEquals(5, i32(d.size()));
};

auto should_operator = []() {
    // Given
    util::Dict<i32, i32> d = {{1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}};
    util::Dict<i32, i32> d2 = {{4, 1}, {5, 1}, {6, 1}, {7, 1}, {8, 1}};

    // When
    auto res = d & d2;  // 交集
    auto res2 = d | d2; // 并集
    auto res3 = d ^ d2; // 相对补集
    auto res4 = d - d2; // 差集

    // Then
    Assertions::assertEquals(2, i32(res.size()));
    Assertions::assertEquals(8, i32(res2.size()));
    Assertions::assertEquals(6, i32(res3.size()));
    Assertions::assertEquals(3, i32(res4.size()));
};

auto should_to_string = []() {
    // Given
    util::Dict<i32, i32> d = {{1, 1}, {2, 1}, {3, 1}};
    util::Dict<CString, i32> d2;
    d2.insert("aaa"_cs, 1);
    d2.insert("bbb"_cs, 3);
    d2.insert("ccc"_cs, 2);

    // When
    CString s = d.__str__();
    CString s2 = d2.__str__();

    // Then
    Assertions::assertEquals("{1:1,2:1,3:1}"_cs, s);
    Assertions::assertEquals("{\"aaa\":1,\"bbb\":3,\"ccc\":2}"_cs, s2);
};

void test_dict() {
    UnitTestGroup group{"test_dict"};

    group.addTest("should_insert", should_insert);
    group.addTest("should_get_or_default", should_get_or_default);
    group.addTest("should_set_default", should_set_default);
    group.addTest("should_update", should_update);
    group.addTest("should_operator", should_operator);
    group.addTest("should_to_string", should_to_string);

    group.startAll();
}

i32 n, k;
std::vector<i32> nums;
std::vector<std::string> strs;

auto setup = []() {
    n = 1e6;
    k = 100;
    for (i32 i = 0; i < n; ++i) {
        nums.push_back(util::Random::instance().next<i32>(0, k));
        strs.push_back(std::to_string(i));
    }
};

auto speed_of_dict_count = []() {
    util::Dict<i32, i32> d;
    for (const auto& num : nums) {
        ++d[num];
    }
};

auto speed_of_unordered_map_count = []() {
    std::unordered_map<i32, i32> mp;
    for (const auto& num : nums) {
        ++mp[num];
    }
};

auto speed_of_dict_insert = []() {
    util::Dict<std::string, i32> d;
    for (i32 i = 0; i < n; ++i) {
        d.insert(strs[i], i);
    }
};

auto speed_of_unordered_map_insert = []() {
    std::unordered_map<std::string, i32> mp;
    for (i32 i = 0; i < n; ++i) {
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

#endif // TEST_DICT_HPP