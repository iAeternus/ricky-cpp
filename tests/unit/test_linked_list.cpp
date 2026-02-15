#include "unit/test_linked_list.hpp"
/**
 * @brief
 * @author Ricky
 * @date 2025/12/16
 * @version 1.0
 */

#include "ricky_test.hpp"
#include "linked_list.hpp"

#include "test/test_registry.hpp"

namespace my::test::test_linked_list {

void should_push_back() {
    // Given
    util::LinkedList<i32> l;

    // When
    l.push_back(1);
    l.push_back(2);
    l.push_back(3);

    // Then
    Assertions::assert_false(l.is_empty());
    Assertions::assert_equals(3ULL, l.size());
    Assertions::assert_equals(1, l.front());
    Assertions::assert_equals(3, l.back());
}

void should_push_front() {
    // Given
    util::LinkedList<i32> l;

    // When
    l.push_front(1);
    l.push_front(2);
    l.push_front(3);

    // Then
    Assertions::assert_false(l.is_empty());
    Assertions::assert_equals(3ULL, l.size());
    Assertions::assert_equals(3, l.front());
    Assertions::assert_equals(1, l.back());
}

void should_pop_front() {
    // Given
    util::LinkedList<i32> l;
    l.push_back(1);
    l.push_back(2);
    l.push_back(3);

    // When
    l.pop_front();

    // Then
    Assertions::assert_equals(2ULL, l.size());
    Assertions::assert_equals(2, l.front());
    Assertions::assert_equals(3, l.back());

    // When
    l.pop_front();
    l.pop_front();

    // Then
    Assertions::assert_true(l.is_empty());
    Assertions::assert_true(l.is_sentinel(l.front()));
    Assertions::assert_true(l.is_sentinel(l.back()));

    // When
    l.pop_front();

    // Then
    Assertions::assert_true(l.is_empty());
    Assertions::assert_true(l.is_sentinel(l.front()));
    Assertions::assert_true(l.is_sentinel(l.back()));
}

void should_pop_back() {
    // Given
    util::LinkedList<i32> l;
    l.push_back(1);
    l.push_back(2);
    l.push_back(3);

    // When
    l.pop_back();

    // Then
    Assertions::assert_equals(2ULL, l.size());
    Assertions::assert_equals(1, l.front());
    Assertions::assert_equals(2, l.back());

    // When
    l.pop_back();
    l.pop_back();

    // Then
    Assertions::assert_true(l.is_empty());
    Assertions::assert_true(l.is_sentinel(l.front()));
    Assertions::assert_true(l.is_sentinel(l.back()));

    // When
    l.pop_back();

    // Then
    Assertions::assert_true(l.is_empty());
    Assertions::assert_true(l.is_sentinel(l.front()));
    Assertions::assert_true(l.is_sentinel(l.back()));
}

void test_contains() {
    // Given
    util::LinkedList<i32> l;
    l.push_back(1);
    l.push_back(2);
    l.push_back(3);

    // When
    auto res = l.contains(1);
    auto res2 = l.contains(0);

    // Then
    Assertions::assert_true(res);
    Assertions::assert_false(res2);
}

void test_linked_list() {
    UnitTestGroup group{"test_linked_list"};

    group.addTest("should_push_back", should_push_back);
    group.addTest("should_push_front", should_push_front);
    group.addTest("should_pop_front", should_pop_front);
    group.addTest("should_pop_back", should_pop_back);
    group.addTest("test_contains", test_contains);

    group.startAll();
}

GROUP_NAME("test_linked_list")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(should_push_back),
    UNIT_TEST_ITEM(should_push_front),
    UNIT_TEST_ITEM(should_pop_front),
    UNIT_TEST_ITEM(should_pop_back),
    UNIT_TEST_ITEM(test_contains))
} // namespace my::test::test_linked_list