#ifndef TEST_BUFFER_HPP
#define TEST_BUFFER_HPP

#include "ricky_test.hpp"
#include "Buffer.hpp"

namespace my::test::test_buffer {

auto should_get_front_and_back = []() {
    // Given
    util::Buffer<int> buf{3};
    buf.append(1), buf.append(2), buf.append(3);

    // When
    auto res = buf.front();
    auto res2 = buf.back();

    // Then
    Assertions::assertEquals(1, res);
    Assertions::assertEquals(3, res2);
};

void test_buffer() {
    UnitTestGroup group{"test_buffer"};

    group.addTest("should_get_front_and_back", should_get_front_and_back);

    group.startAll();
}

} // namespace my::test::test_buffer

#endif // TEST_BUFFER_HPP