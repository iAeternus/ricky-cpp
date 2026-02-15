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
    Assertions::assertEquals(3ULL, d.size());
    Assertions::assertTrue(d.contains("aaa"_cs));
    Assertions::assertFalse(d.contains("ddd"_cs));

    // When
    d.insert("ccc"_cs, 0);

    // Then
    Assertions::assertEquals(3ULL, d.size());
    Assertions::assertEquals(0, d.get("ccc"_cs));
}

void should_get_or_default() {
    // Given
    util::HashMap<i32, i32> d = {{1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}};

    // When
    i32 res1 = d.get(1);
    auto& res2 = d.get_or_default(2, 0);
    auto& res3 = d.get_or_default(6, 0); // TODO [should_get_or_default] Test failed! Exception: Assertion Failed: Expected 0, but got 1998353488

    // Then
    Assertions::assertEquals(1, res1);
    Assertions::assertEquals(1, res2);
    Assertions::assertEquals(0, res3);
}

void should_fail_to_get_if_key_not_found() {
    // Given
    util::HashMap<i32, i32> d = {{1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}};

    // When & Then
    Assertions::assertThrows("key '6' not found in hash_map", [d]() {
        d.get(6);
    });
}

void should_set_default() {
    // Given
    util::HashMap<i32, i32> d;

    // When
    d.set_default(1, 1);

    // Then
    Assertions::assertEquals(1, d.get(1));

    // When
    d.set_default(1, 2);

    // Then
    Assertions::assertEquals(1, d.get(1));
}

void should_update() {
    // Given
    util::HashMap<i32, i32> d = {{1, 1}, {2, 1}, {3, 1}};

    // When
    d.update({{4, 1}, {5, 1}});

    // Then
    Assertions::assertEquals(5ULL, d.size());
}

void should_remove() {
    // Given
    util::HashMap<i32, i32> d = {{1, 1}, {2, 1}, {3, 1}};

    // When
    d.remove(1);

    // Then
    Assertions::assertEquals(2ULL, d.size());
    Assertions::assertFalse(d.contains(1));
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
    Assertions::assertEquals(2, i32(res.size()));
    Assertions::assertEquals(8, i32(res2.size()));
    Assertions::assertEquals(6, i32(res3.size()));
    Assertions::assertEquals(3, i32(res4.size()));
}

void should_to_string() {
    // Given
    util::HashMap<i32, i32> d = {{1, 1}, {2, 1}, {3, 1}};
    util::HashMap<CString, i32> d2;
    d2.insert("aaa"_cs, 1);
    d2.insert("bbb"_cs, 3);
    d2.insert("ccc"_cs, 2);

    // When
    CString s = d.__str__();
    CString s2 = d2.__str__();

    // Then
    Assertions::assertEquals("{1:1,2:1,3:1}"_cs, s);
    Assertions::assertEquals("{\"aaa\":1,\"bbb\":3,\"ccc\":2}"_cs, s2);
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
