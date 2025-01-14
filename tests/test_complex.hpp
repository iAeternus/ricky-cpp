#ifndef TEST_COMPLEX_HPP
#define TEST_COMPLEX_HPP

#include "ricky_test.hpp"
#include "Complex.hpp"

namespace my::test::test_complex {

auto should_construct = []() {
    math::Complex c{1, 2};
    math::Complex c2 = "2";
    math::Complex c3 = "2i";
    math::Complex c4 = "-1+2i";
    math::Complex c5 = "-1-2I";
    math::Complex c6 = " 1 + 2i ";
    math::Complex c7 = "0";
    math::Complex c8 = "1 + i";
    math::Complex c9 = "i";
    Assertions::assertEquals("1+2i"_cs, c.__str__());
    Assertions::assertEquals("2"_cs, c2.__str__());
    Assertions::assertEquals("2i"_cs, c3.__str__());
    Assertions::assertEquals("-1+2i"_cs, c4.__str__());
    Assertions::assertEquals("-1-2i"_cs, c5.__str__());
    Assertions::assertEquals("1+2i"_cs, c6.__str__());
    Assertions::assertEquals("0"_cs, c7.__str__());
    Assertions::assertEquals("1+i"_cs, c8.__str__());
    Assertions::assertEquals("i"_cs, c9.__str__());
};

auto should_calculate_norm = []() {
    // Given
    math::Complex a = "3 + 4i";

    // When
    double res = a.norm();

    // Then
    Assertions::assertEquals(5.0, res);
};

auto should_calculate_arg = []() {
    // Given
    math::Complex a = "3 + 4i";

    // When
    double res = a.arg();

    // Then
    Assertions::assertEquals(0.9272952180016122, res);
};

auto should_add = []() {
    // Given
    math::Complex a = "1 + 2i", b = "1 - 2i";

    // When
    auto res = a + b;
    a += b;

    // Then
    Assertions::assertEquals("2"_cs, res.__str__());
    Assertions::assertEquals(res.__str__(), a.__str__());
};

auto should_subtract = []() {
    // Given
    math::Complex a = "1 + 2i", b = "1 - 2i";

    // When
    auto res = a - b;
    a -= b;

    // Then
    Assertions::assertEquals("4i"_cs, res.__str__());
    Assertions::assertEquals(res.__str__(), a.__str__());
};

auto should_multiply = []() {
    // Given
    math::Complex a = "1 + 2i", b = "1 - 2i";

    // When
    auto res = a * b;
    a *= b;

    // Then
    Assertions::assertEquals("5"_cs, res.__str__());
    Assertions::assertEquals(res.__str__(), a.__str__());
};

auto should_divide = []() {
    // Given
    math::Complex a = "1 + 2i", b = "1 - 2i";

    // When
    auto res = a / b;
    a /= b;

    // Then
    Assertions::assertEquals("-0.6+0.8i"_cs, res.__str__());
    Assertions::assertEquals(res.__str__(), a.__str__());
};

void test_complex() {
    UnitTestGroup group{"test_complex"};

    group.addTest("should_construct", should_construct);
    group.addTest("should_calculate_norm", should_calculate_norm);
    group.addTest("should_calculate_arg", should_calculate_arg);
    group.addTest("should_add", should_add);
    group.addTest("should_subtract", should_subtract);
    group.addTest("should_multiply", should_multiply);
    group.addTest("should_divide", should_divide);

    group.startAll();
}

} // namespace my::test::test_complex

#endif // TEST_COMPLEX_HPP