#include "test_pair.hpp"
#include "my_pair.hpp"
#include "ricky_test.hpp"

namespace my::test::test_pair {

void should_construct() {
    // Given
    i32 a = 10;
    f64 b = 3.14;
    CString c = "abc";

    // When
    Pair<i32, f64> p = {a, b};
    Pair<i32, CString> p2{a, c};

    // Then
    Assertions::assertEquals("(10, 3.14)"_cs, p.__str__());
    Assertions::assertEquals("(10, \"abc\")"_cs, p2.__str__());

    // When
    Pair<i32, CString> p3{p2};
    Pair<i32, CString> p4 = p3;

    // Then
    Assertions::assertEquals("(10, \"abc\")"_cs, p3.__str__());
    Assertions::assertEquals("(10, \"abc\")"_cs, p4.__str__());
}

void should_structured_binding() {
    // Given
    Pair<i32, i32> p = {1, 2};

    // When
    auto [x, y] = p;

    // Then
    Assertions::assertEquals(1, x);
    Assertions::assertEquals(2, y);
}

GROUP_NAME("test_pair")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(should_construct),
    UNIT_TEST_ITEM(should_structured_binding))

} // namespace my::test::test_pair