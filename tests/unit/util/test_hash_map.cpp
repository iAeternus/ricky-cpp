#include "test_hash_map.hpp"
#include "hash_map.hpp"
#include "random.hpp"
#include "ricky_test.hpp"

namespace my::test::test_hash_map {

void should_insert() {
    // Given
    util::HashMap<CString, i32> d;

    // When
    d.insert("aaa"_cs, 1);
    d.insert("bbb"_cs, 3);
    d.insert("ccc"_cs, 2);

    // Then
    Assertions::assert_equals(3ULL, d.size());
    Assertions::assert_true(d.contains("aaa"_cs));
    Assertions::assert_false(d.contains("ddd"_cs));

    // When
    d.insert("ccc"_cs, 0);

    // Then
    Assertions::assert_equals(3ULL, d.size());
    Assertions::assert_equals(0, d.get("ccc"_cs));
}

void should_get_or_default() {
    // Given
    util::HashMap<i32, i32> d = {{1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}};

    // When
    i32 res1 = d.get(1);
    auto& res2 = d.get_or_default(2, 0);
    auto& res3 = d.get_or_default(6, 0); // TODO [should_get_or_default] Test failed! Exception: Assertion Failed: Expected 0, but got 1998353488

    // Then
    Assertions::assert_equals(1, res1);
    Assertions::assert_equals(1, res2);
    Assertions::assert_equals(0, res3);
}

void should_fail_to_get_if_key_not_found() {
    // Given
    util::HashMap<i32, i32> d = {{1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}};

    // When & Then
    Assertions::assert_throws("key '6' not found in hash_map", [d]() {
        d.get(6);
    });
}

void should_set_default() {
    // Given
    util::HashMap<i32, i32> d;

    // When
    d.set_default(1, 1);

    // Then
    Assertions::assert_equals(1, d.get(1));

    // When
    d.set_default(1, 2);

    // Then
    Assertions::assert_equals(1, d.get(1));
}

void should_update() {
    // Given
    util::HashMap<i32, i32> d = {{1, 1}, {2, 1}, {3, 1}};

    // When
    d.update({{4, 1}, {5, 1}});

    // Then
    Assertions::assert_equals(5ULL, d.size());
}

void should_remove() {
    // Given
    util::HashMap<i32, i32> d = {{1, 1}, {2, 1}, {3, 1}};

    // When
    d.remove(1);

    // Then
    Assertions::assert_equals(2ULL, d.size());
    Assertions::assert_false(d.contains(1));
}

void should_operator() {
    // Given
    util::HashMap<i32, i32> d = {{1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}};
    util::HashMap<i32, i32> d2 = {{4, 1}, {5, 1}, {6, 1}, {7, 1}, {8, 1}};

    // When
    auto res = d & d2;  // 交集
    auto res2 = d | d2; // 并集
    auto res3 = d ^ d2; // 相对补集
    auto res4 = d - d2; // 差集

    // Then
    Assertions::assert_equals(2, i32(res.size()));
    Assertions::assert_equals(8, i32(res2.size()));
    Assertions::assert_equals(6, i32(res3.size()));
    Assertions::assert_equals(3, i32(res4.size()));
}

void should_to_string() {
    // Given
    util::HashMap<i32, i32> d = {{1, 1}, {2, 1}, {3, 1}};
    util::HashMap<CString, i32> d2;
    d2.insert("aaa"_cs, 1);
    d2.insert("bbb"_cs, 3);
    d2.insert("ccc"_cs, 2);

    // When
    CString s = d.to_string();
    CString s2 = d2.to_string();

    // Then
    Assertions::assert_equals("{1:1,2:1,3:1}"_cs, s);
    Assertions::assert_equals("{\"aaa\":1,\"bbb\":3,\"ccc\":2}"_cs, s2);
}

GROUP_NAME("test_hash_map")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(should_insert),
    UNIT_TEST_ITEM(should_get_or_default),
    UNIT_TEST_ITEM(should_fail_to_get_if_key_not_found),
    UNIT_TEST_ITEM(should_set_default),
    UNIT_TEST_ITEM(should_update),
    UNIT_TEST_ITEM(should_remove),
    UNIT_TEST_ITEM(should_operator),
    UNIT_TEST_ITEM(should_to_string))
} // namespace my::test::test_hash_map

