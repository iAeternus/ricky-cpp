#ifndef TEST_BUFFER_HPP
#define TEST_BUFFER_HPP

#include "ricky_test.hpp"
#include "buffer.hpp"

namespace my::test::test_buffer {

auto should_get_front_and_back = []() {
    // Given
    util::Buffer<i32> buf{3};
    buf.push(1), buf.push(2), buf.push(3);

    // When
    auto res = buf.first();
    auto res2 = buf.last();

    // Then
    Assertions::assertEquals(1, res);
    Assertions::assertEquals(3, res2);
};

auto test_buffer() {
    UnitTestGroup group{"test_buffer"};

    group.addTest("should_get_front_and_back", should_get_front_and_back);

    group.startAll();
}

} // namespace my::test::test_buffer

#endif // TEST_BUFFER_HPP