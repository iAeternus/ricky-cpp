#ifndef TEST_COMPLEX_HPP
#define TEST_COMPLEX_HPP

#include "UnitTest.hpp"
#include "Assertions.hpp"
#include "Complex.hpp"

namespace my::test::test_complex {

fn should_construct = []() {
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

fn should_calc_modulus = []() {
    // Given
    math::Complex a = "3 + 4i";

    // When
    f64 res = a.modulus();

    // Then
    Assertions::assertEquals(5.0, res);
};

fn should_calc_arg = []() {
    // Given
    math::Complex a = "3 + 4i";

    // When
    f64 res = a.angle();

    // Then
    Assertions::assertEquals(0.9272952180016122, res);
};

fn should_add = []() {
    // Given
    math::Complex a = "1 + 2i", b = "1 - 2i";

    // When
    auto res = a + b;
    a += b;

    // Then
    Assertions::assertEquals("2"_cs, res.__str__());
    Assertions::assertEquals(res.__str__(), a.__str__());
};

fn should_subtract = []() {
    // Given
    math::Complex a = "1 + 2i", b = "1 - 2i";

    // When
    auto res = a - b;
    a -= b;

    // Then
    Assertions::assertEquals("4i"_cs, res.__str__());
    Assertions::assertEquals(res.__str__(), a.__str__());
};

fn should_multiply = []() {
    // Given
    math::Complex a = "1 + 2i", b = "1 - 2i";

    // When
    auto res = a * b;
    a *= b;

    // Then
    Assertions::assertEquals("5"_cs, res.__str__());
    Assertions::assertEquals(res.__str__(), a.__str__());
};

fn should_divide = []() {
    // Given
    math::Complex a = "1 + 2i", b = "1 - 2i";

    // When
    auto res = a / b;
    a /= b;

    // Then
    Assertions::assertEquals("-0.6+0.8i"_cs, res.__str__());
    Assertions::assertEquals(res.__str__(), a.__str__());
};

fn test_complex() {
    UnitTestGroup group{"test_complex"};

    group.addTest("should_construct", should_construct);
    group.addTest("should_calc_modulus", should_calc_modulus);
    group.addTest("should_calc_arg", should_calc_arg);
    group.addTest("should_add", should_add);
    group.addTest("should_subtract", should_subtract);
    group.addTest("should_multiply", should_multiply);
    group.addTest("should_divide", should_divide);

    group.startAll();
}

} // namespace my::test::test_complex

#endif // TEST_COMPLEX_HPP