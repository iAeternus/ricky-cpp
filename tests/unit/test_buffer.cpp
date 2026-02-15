#include "unit/test_buffer.hpp"

#include "ricky_test.hpp"
#include "buffer.hpp"

#include "test/test_registry.hpp"

namespace my::test::test_buffer {

void should_get_front_and_back() {
    // Given
    util::Buffer<i32> buf{3};
    buf.push(1), buf.push(2), buf.push(3);

    // When
    auto res = buf.first();
    auto res2 = buf.last();

    // Then
    Assertions::assertEquals(1, res);
    Assertions::assertEquals(3, res2);
}

void test_buffer() {
    UnitTestGroup group{"test_buffer"};

    group.addTest("should_get_front_and_back", should_get_front_and_back);

    group.startAll();
}

GROUP_NAME("test_buffer")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(should_get_front_and_back))
} // namespace my::test::test_buffer