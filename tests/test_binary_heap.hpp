#ifndef TEST_PRIORITY_QUEUE_HPP
#define TEST_PRIORITY_QUEUE_HPP

#include "ricky_test.hpp"
#include "binary_heap.hpp"

namespace my::test::test_binary_heap {

auto it_works = []() {
    // Given
    util::BinaryHeap<i32> bh;

    // When
    bh.push(3);
    bh.push(1);
    bh.push(4);
    bh.push(2);

    // Then
    Assertions::assertEquals(4, bh.size());
    Assertions::assertFalse(bh.empty());
    Assertions::assertEquals(1, bh.top());

    // When
    bh.pop();

    // Then
    Assertions::assertEquals(3, bh.size());
    Assertions::assertEquals(2, bh.top());
};

auto it_works2 = []() {
    // Given
    util::BinaryHeap<i32, std::greater<>> bh;

    // When
    bh.push(3);
    bh.push(1);
    bh.push(4);
    bh.push(2);

    // Then
    Assertions::assertEquals(4, bh.size());
    Assertions::assertFalse(bh.empty());
    Assertions::assertEquals(4, bh.top());

    // When
    bh.pop();

    // Then
    Assertions::assertEquals(3, bh.size());
    Assertions::assertEquals(3, bh.top());
};

auto should_heapify = []() {
    // Given
    util::Vec<i32> v = {3, 4, 5, 6, 1, 7, 8};

    // When
    util::BinaryHeap<i32, std::greater<i32>> bh{v};

    // Then
    Assertions::assertEquals("[8,6,7,4,1,3,5]"_cs, bh.__str__());
};

auto test_binary_heap() {
    UnitTestGroup group{"test_binary_heap"};

    group.addTest("it_works", it_works);
    group.addTest("it_works2", it_works2);
    group.addTest("should_heapify", should_heapify);

    group.startAll();
}

} // namespace my::test::test_binary_heap

#endif // TEST_PRIORITY_QUEUE_HPP