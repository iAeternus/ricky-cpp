#include "test_binary_heap.hpp"
#include "binary_heap.hpp"
#include "ricky_test.hpp"

namespace my::test::test_binary_heap {

void it_works() {
    // Given
    util::BinaryHeap<i32> bh;

    // When
    bh.push(3);
    bh.push(1);
    bh.push(4);
    bh.push(2);

    // Then
    Assertions::assert_equals(4, bh.size());
    Assertions::assert_false(bh.is_empty());
    Assertions::assert_equals(1, bh.top());

    // When
    bh.pop();

    // Then
    Assertions::assert_equals(3, bh.size());
    Assertions::assert_equals(2, bh.top());
}

void it_works2() {
    // Given
    util::BinaryHeap<i32, std::greater<>> bh;

    // When
    bh.push(3);
    bh.push(1);
    bh.push(4);
    bh.push(2);

    // Then
    Assertions::assert_equals(4, bh.size());
    Assertions::assert_false(bh.is_empty());
    Assertions::assert_equals(4, bh.top());

    // When
    bh.pop();

    // Then
    Assertions::assert_equals(3, bh.size());
    Assertions::assert_equals(3, bh.top());
}

void should_heapify() {
    // Given
    util::Vec<i32> v = {3, 4, 5, 6, 1, 7, 8};

    // When
    util::BinaryHeap<i32, std::greater<i32>> bh{v};

    // Then
    Assertions::assert_equals("[8,6,7,4,1,3,5]"_cs, bh.to_string());
}

GROUP_NAME("test_binary_heap")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(it_works),
    UNIT_TEST_ITEM(it_works2),
    UNIT_TEST_ITEM(should_heapify))

} // namespace my::test::test_binary_heap
