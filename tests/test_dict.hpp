#ifndef TEST_DICT_HPP
#define TEST_DICT_HPP

#include "UnitTest.hpp"
#include "Assertions.hpp"
#include "Dict.hpp"
#include "Random.hpp"

namespace my::test::test_dict {

fn should_insert = []() {
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

fn should_get_or_default = []() {
    // Given
    util::Dict<i32, i32> d = {{1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}};

    // When
    i32 res1 = d.get(1);
    auto& res2 = d.get_or_default(2, 0);
    auto& res3 = d.get_or_default(6, 0); // TODO [should_get_or_default] Test failed! Exception: Assertion Failed: Expected 0, but got 1998353488

    // Then
    Assertions::assertEquals(1, res1);
    Assertions::assertEquals(1, res2);
    Assertions::assertEquals(0, res3);
};

fn should_fail_to_get_if_key_not_found = []() {
    // Given
    util::Dict<i32, i32> d = {{1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}};

    // When & Then
    Assertions::assertThrows("key '6' not found in dict", [d]() {
        d.get(6);
    });
};

fn should_set_default = []() {
    // Given
    util::Dict<i32, i32> d;

    // When
    d.set_default(1, 1);

    // Then
    Assertions::assertEquals(1, d.get(1));

    // When
    d.set_default(1, 2);

    // Then
    Assertions::assertEquals(1, d.get(1));
};

fn should_update = []() {
    // Given
    util::Dict<i32, i32> d = {{1, 1}, {2, 1}, {3, 1}};

    // When
    d.update({{4, 1}, {5, 1}});

    // Then
    Assertions::assertEquals(5, i32(d.size()));
};

fn should_remove = []() {
    // Given
    util::Dict<i32, i32> d = {{1, 1}, {2, 1}, {3, 1}};

    // When
    d.remove(1);

    // Then
    Assertions::assertEquals(2, d.size());
    Assertions::assertFalse(d.contains(1));
};

fn should_operator = []() {
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

fn should_to_string = []() {
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

fn test_dict() {
    UnitTestGroup group{"test_dict"};

    group.addTest("should_insert", should_insert);
    group.addTest("should_get_or_default", should_get_or_default);
    group.addTest("should_fail_to_get_if_key_not_found", should_fail_to_get_if_key_not_found);
    group.addTest("should_set_default", should_set_default);
    group.addTest("should_update", should_update);
    group.addTest("should_remove", should_remove);
    group.addTest("should_operator", should_operator);
    group.addTest("should_to_string", should_to_string);

    group.startAll();
}

inline i32 n, k;
inline std::vector<i32> nums;
inline std::vector<std::string> strs;

fn setup = []() {
    n = 1e6;
    k = 100;
    for (usize i = 0; i < n; ++i) {
        nums.push_back(util::Random::instance().next<i32>(0, k));
        strs.push_back(std::to_string(i));
    }
};

fn speed_of_dict_count = []() {
    util::Dict<i32, i32> d;
    for (const auto& num : nums) {
        ++d[num];
    }
};

fn speed_of_unordered_map_count = []() {
    std::unordered_map<i32, i32> mp;
    for (const auto& num : nums) {
        ++mp[num];
    }
};

fn speed_of_dict_insert = []() {
    util::Dict<std::string, i32> d;
    for (i32 i = 0; i < n; ++i) {
        d.insert(strs[i], i);
    }
};

fn speed_of_unordered_map_insert = []() {
    std::unordered_map<std::string, i32> mp;
    for (i32 i = 0; i < n; ++i) {
        mp.insert(std::make_pair(strs[i], i));
    }
};

fn test_dict_speed() {
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