#ifndef TEST_PRIORITY_QUEUE_HPP
#define TEST_PRIORITY_QUEUE_HPP

#include "Assertions.hpp"
#include "UnitTest.hpp"
#include "PriorityQueue.hpp"

namespace my::test::test_priority_queue {

fn it_works = []() {
    // Given
    util::PriorityQueue<i32> pq;

    // When
    pq.push(3);
    pq.push(1);
    pq.push(4);
    pq.push(2);

    // Then
    Assertions::assertEquals(4, pq.size());
    Assertions::assertFalse(pq.empty());
    Assertions::assertEquals(1, pq.top());

    // When
    pq.pop();

    // Then
    Assertions::assertEquals(3, pq.size());
    Assertions::assertEquals(2, pq.top());
};

fn it_works2 = []() {
    // Given
    util::PriorityQueue<i32, std::greater<>> pq;

    // When
    pq.push(3);
    pq.push(1);
    pq.push(4);
    pq.push(2);

    // Then
    Assertions::assertEquals(4, pq.size());
    Assertions::assertFalse(pq.empty());
    Assertions::assertEquals(4, pq.top());

    // When
    pq.pop();

    // Then
    Assertions::assertEquals(3, pq.size());
    Assertions::assertEquals(3, pq.top());
};

fn should_heapify = []() {
    // Given
    util::Vec<i32> v = {3, 4, 5, 6, 1, 7, 8};

    // When
    util::PriorityQueue<i32, std::greater<i32>> pq{v};

    // Then
    Assertions::assertEquals("[8,6,7,4,1,3,5]"_cs, pq.__str__());
};

fn test_priority_queue() {
    UnitTestGroup group{"test_priority_queue"};

    group.addTest("it_works", it_works);
    group.addTest("it_works2", it_works2);
    group.addTest("should_heapify", should_heapify);

    group.startAll();
}

} // namespace my::test::test_priority_queue

#endif // TEST_PRIORITY_QUEUE_HPP