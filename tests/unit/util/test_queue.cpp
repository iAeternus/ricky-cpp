#include "test_queue.hpp"
#include "link_list_queue.hpp"
#include "ricky_test.hpp"

namespace my::test::test_queue {

void it_works() {
    util::Queue<i32> q;
    Assertions::assert_true(q.empty());

    q.push(1), q.push(2), q.push(3), q.push(4);
    Assertions::assert_false(q.empty());
    Assertions::assert_equals(4, q.size());
    Assertions::assert_equals(1, q.front());
    Assertions::assert_equals(4, q.tail());

    q.pop();
    Assertions::assert_equals(3, q.size());
    Assertions::assert_equals(2, q.front());

    q.pop();
    Assertions::assert_equals(2, q.size());
    Assertions::assert_equals(3, q.front());

    q.pop();
    Assertions::assert_equals(1, q.size());
    Assertions::assert_equals(4, q.front());

    q.pop();
    Assertions::assert_equals(0, q.size());
}

void should_fail_to_pop_if_queue_is_empty() {
    // Given
    util::Queue<i32> q;

    // When & Then
    Assertions::assert_throws("Queue is is_empty.", [&]() {
        q.pop();
    });
}

void should_fail_to_get_front_if_queue_is_empty() {
    // Given
    util::Queue<i32> q;

    // When & Then
    Assertions::assert_throws("Queue is is_empty.", [&]() {
        q.front();
    });
}

void should_fail_to_get_tail_if_queue_is_empty() {
    // Given
    util::Queue<i32> q;

    // When & Then
    Assertions::assert_throws("Queue is is_empty.", [&]() {
        q.tail();
    });
}

GROUP_NAME("test_queue")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(it_works),
    UNIT_TEST_ITEM(should_fail_to_pop_if_queue_is_empty),
    UNIT_TEST_ITEM(should_fail_to_get_front_if_queue_is_empty),
    UNIT_TEST_ITEM(should_fail_to_get_tail_if_queue_is_empty))

} // namespace my::test::test_queue