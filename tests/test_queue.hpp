#ifndef TEST_QUEUE_HPP
#define TEST_QUEUE_HPP

#include "UnitTest.hpp"
#include "Assertions.hpp"
#include "Queue.hpp"
#include "Random.hpp"

#include <queue>

namespace my::test::test_queue {

fn it_works = []() {
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

fn test_queue() {
    UnitTestGroup group{"test_queue"};

    group.addTest("it_works", it_works);

    group.startAll();
}

constexpr i32 N = 1e6;

fn speed_of_util_queue_push_and_pop = []() {
    util::Queue<CString> q;
    for (usize i = 0; i < N; ++i) {
        q.push(util::Random::instance().next_str(3));
    }
    while (!q.empty()) {
        q.pop();
    }
};

fn speed_of_std_queue_push_and_pop = []() {
    std::queue<CString> q;
    for (usize i = 0; i < N; ++i) {
        q.push(util::Random::instance().next_str(3));
    }
    while (!q.empty()) {
        q.pop();
    }
};

fn test_queue_speed() {
    UnitTestGroup group{"test_queue_speed"};

    group.addTest("speed_of_util_queue_push_and_pop", speed_of_util_queue_push_and_pop);
    group.addTest("speed_of_std_queue_push_and_pop", speed_of_std_queue_push_and_pop);

    group.startAll();
}

} // namespace my::test::test_queue

#endif // TEST_QUEUE_HPP