#ifndef TEST_MATH_UTILS_HPP
#define TEST_MATH_UTILS_HPP

#include "ricky_test.hpp"
#include "math_utils.hpp"

namespace my::test::test_math_utils {

auto should_cal_gcd = []() {
    // Given
    i32 a = 2, b = 4, c = 5, d = 2;

    // When
    i32 res = math::gcd(a, b);
    i32 res2 = math::gcd(a, c);
    i32 res3 = math::gcd(a, d);
    i32 res4 = math::gcd(a, math::gcd(b, c));

    // Then
    Assertions::assertEquals(2, res);
    Assertions::assertEquals(1, res2);
    Assertions::assertEquals(2, res3);
    Assertions::assertEquals(1, res4);
};

auto should_cal_lcm = []() {
    // Given
    i32 a = 2, b = 4, c = 5, d = 2;

    // When
    i32 res = math::lcm(a, b);
    i32 res2 = math::lcm(a, c);
    i32 res3 = math::lcm(a, d);
    i32 res4 = math::lcm(a, math::lcm(b, c));

    // Then
    Assertions::assertEquals(4, res);
    Assertions::assertEquals(10, res2);
    Assertions::assertEquals(2, res3);
    Assertions::assertEquals(20, res4);
};


void test_math_utils() {
    UnitTestGroup group{"test_math_utils"};

    group.addTest("should_cal_gcd", should_cal_gcd);
    group.addTest("should_cal_lcm", should_cal_lcm);

    group.startAll();
}

} // namespace my::test::test_math_utils

#endif // TEST_MATH_UTILS_HPP