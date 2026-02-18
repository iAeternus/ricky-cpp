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

void should_copy_and_move() {
    Pair<i32, CString> p1{1, "abc"};
    Pair<i32, CString> p2{p1};
    Pair<i32, CString> p3 = p2;

    Assertions::assertEquals("(1, \"abc\")"_cs, p2.__str__());
    Assertions::assertEquals("(1, \"abc\")"_cs, p3.__str__());

    Pair<i32, CString> p4{std::move(p3)};
    Pair<i32, CString> p5;
    p5 = std::move(p4);

    Assertions::assertEquals("(1, \"abc\")"_cs, p5.__str__());
}

void should_cmp() {
    Pair<i32, i32> p1{1, 2};
    Pair<i32, i32> p2{1, 3};
    Pair<i32, i32> p3{2, 0};

    Assertions::assertTrue(p1.__cmp__(p2) < 0);
    Assertions::assertTrue(p2.__cmp__(p1) > 0);
    Assertions::assertTrue(p1.__cmp__(p1) == 0);
    Assertions::assertTrue(p3.__cmp__(p1) > 0);
}

void should_get() {
    Pair<i32, i32> p{4, 5};
    Assertions::assertEquals(4, p.get<0>());
    Assertions::assertEquals(5, p.get<1>());

    auto& a = my::get<0>(p);
    auto& b = my::get<1>(p);
    a = 7;
    b = 8;

    Assertions::assertEquals(7, p.first());
    Assertions::assertEquals(8, p.second());
}

GROUP_NAME("test_pair")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(should_construct),
    UNIT_TEST_ITEM(should_structured_binding),
    UNIT_TEST_ITEM(should_copy_and_move),
    UNIT_TEST_ITEM(should_cmp),
    UNIT_TEST_ITEM(should_get))

} // namespace my::test::test_pair
