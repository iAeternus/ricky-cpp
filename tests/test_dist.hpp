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

void test_dict() {
    UnitTestGroup group{"test_dict"};

    group.addTest("should_insert", should_insert);
    group.addTest("should_get_or_default", should_get_or_default);
    group.addTest("should_set_default", should_set_default);
    group.addTest("should_update", should_update);

    group.startAll();
}

} // namespace my::test::test_dict

#endif // TEST_DIST_HPP