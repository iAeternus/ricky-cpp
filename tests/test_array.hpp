#ifndef TEST_ARRAY_HPP
#define TEST_ARRAY_HPP

#include "ricky_test.hpp"
#include "Array.hpp"

namespace my::test::test_array {

auto should_at = []() {
    // Given
    util::Array<char> arr = {'a', 'b', 'c'};

    // When
    auto res = arr.at(0);

    // Then
    Assertions::assertEquals('a', res);

    // When
    arr.at(0) = arr.at(1);

    // Then
    Assertions::assertEquals("[b,b,c]"_cs, arr.__str__());
};

void test_array() {
    UnitTestGroup group{"test_array"};

    group.addTest("should_at", should_at);

    group.startAll();
}

} // namespace my::test::test_array

#endif // TEST_ARRAY_HPP