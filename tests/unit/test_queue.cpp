#include "test_queue.hpp"
#include "link_list_queue.hpp"
#include "random.hpp"
#include "ricky_test.hpp"

#include <queue>

namespace my::test::test_queue {

void it_works() {
    util::Queue<i32> q;
    Assertions::assertTrue(q.empty());

    q.push(1), q.push(2), q.push(3), q.push(4);
    Assertions::assertFalse(q.empty());
    Assertions::assertEquals(4, q.size());
    Assertions::assertEquals(1, q.front());
    Assertions::assertEquals(4, q.tail());

    q.pop();
    Assertions::assertEquals(3, q.size());
    Assertions::assertEquals(2, q.front());

    q.pop();
    Assertions::assertEquals(2, q.size());
    Assertions::assertEquals(3, q.front());

    q.pop();
    Assertions::assertEquals(1, q.size());
    Assertions::assertEquals(4, q.front());

    q.pop();
    Assertions::assertEquals(0, q.size());
}

void should_fail_to_pop_if_queue_is_empty() {
    // Given
    util::Queue<i32> q;

    // When & Then
    Assertions::assertThrows("Queue is is_empty.", [&]() {
        q.pop();
    });
}

void should_fail_to_get_front_if_queue_is_empty() {
    // Given
    util::Queue<i32> q;

    // When & Then
    Assertions::assertThrows("Queue is is_empty.", [&]() {
        q.front();
    });
}

void should_fail_to_get_tail_if_queue_is_empty() {
    // Given
    util::Queue<i32> q;

    // When & Then
    Assertions::assertThrows("Queue is is_empty.", [&]() {
        q.tail();
    });
}

constexpr i32 N = 1e6;

void speed_of_util_queue_push_and_pop() {
    util::Queue<CString> q;
    for (usize i = 0; i < N; ++i) {
        q.push(util::Random::instance().next_str(3));
    }
    while (!q.empty()) {
        q.pop();
    }
}

void speed_of_std_queue_push_and_pop() {
    std::queue<CString> q;
    for (usize i = 0; i < N; ++i) {
        q.push(util::Random::instance().next_str(3));
    }
    while (!q.empty()) {
        q.pop();
    }
}

void test_queue_speed() {
    speed_of_util_queue_push_and_pop();
    speed_of_std_queue_push_and_pop();
}

GROUP_NAME("test_queue")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(it_works),
    UNIT_TEST_ITEM(should_fail_to_pop_if_queue_is_empty),
    UNIT_TEST_ITEM(should_fail_to_get_front_if_queue_is_empty),
    UNIT_TEST_ITEM(should_fail_to_get_tail_if_queue_is_empty))

} // namespace my::test::test_queue