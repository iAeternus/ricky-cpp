#ifndef TEST_QUEUE_HPP
#define TEST_QUEUE_HPP

#include "ricky_test.hpp"
#include "link_list_queue.hpp"
#include "random.hpp"

#include <queue>

namespace my::test::test_queue {

auto it_works = []() {
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
};

auto should_fail_to_pop_if_queue_is_empty = []() {
    // Given
    util::Queue<i32> q;

    // When & Then
    Assertions::assertThrows("Queue is empty.", [&]() {
        q.pop();
    });
};

auto should_fail_to_get_front_if_queue_is_empty = []() {
    // Given
    util::Queue<i32> q;

    // When & Then
    Assertions::assertThrows("Queue is empty.", [&]() {
        q.front();
    });
};

auto should_fail_to_get_tail_if_queue_is_empty = []() {
    // Given
    util::Queue<i32> q;

    // When & Then
    Assertions::assertThrows("Queue is empty.", [&]() {
        q.tail();
    });
};

auto test_queue() {
    UnitTestGroup group{"test_queue"};

    group.addTest("it_works", it_works);
    group.addTest("should_fail_to_pop_if_queue_is_empty", should_fail_to_pop_if_queue_is_empty);
    group.addTest("should_fail_to_get_front_if_queue_is_empty", should_fail_to_get_front_if_queue_is_empty);
    group.addTest("should_fail_to_get_tail_if_queue_is_empty", should_fail_to_get_tail_if_queue_is_empty);

    group.startAll();
}

constexpr i32 N = 1e6;

auto speed_of_util_queue_push_and_pop = []() {
    util::Queue<CString> q;
    for (usize i = 0; i < N; ++i) {
        q.push(util::Random::instance().next_str(3));
    }
    while (!q.empty()) {
        q.pop();
    }
};

auto speed_of_std_queue_push_and_pop = []() {
    std::queue<CString> q;
    for (usize i = 0; i < N; ++i) {
        q.push(util::Random::instance().next_str(3));
    }
    while (!q.empty()) {
        q.pop();
    }
};

auto test_queue_speed() {
    UnitTestGroup group{"test_queue_speed"};

    group.addTest("speed_of_util_queue_push_and_pop", speed_of_util_queue_push_and_pop);
    group.addTest("speed_of_std_queue_push_and_pop", speed_of_std_queue_push_and_pop);

    group.startAll();
}

} // namespace my::test::test_queue

#endif // TEST_QUEUE_HPP