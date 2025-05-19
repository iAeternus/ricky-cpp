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

auto should_judge_is_prime = []() {
    // Given
    i32 a = 1, b = 2, c = 3, d = 4;

    // When
    bool res = math::isprime(a);
    bool res2 = math::isprime(b);
    bool res3 = math::isprime(c);
    bool res4 = math::isprime(d);

    // Then
    Assertions::assertFalse(res);
    Assertions::assertTrue(res2);
    Assertions::assertTrue(res3);
    Assertions::assertFalse(res4);
};

auto should_cal_pow = []() {
    // Given
    i32 a = 2, n = 10, n2 = 0;

    // When
    i32 res = math::pow(a, n);
    i32 res2 = math::pow(a, n2);

    // Then
    Assertions::assertEquals(1024, res);
    Assertions::assertEquals(1, res2);
};

auto should_compare = []() {
    // Given
    f64 a = 1.0, b = 1.0;
    // f64 c = 1.000000000000001, d = 0.999999999999999;   // 1e-15
    f64 c = 1.0000001, d = 0.9999999; // 1e-7
    // f64 e = 1.0000000000000001, f = 0.9999999999999999; // 1e-16
    f64 e = 1.000000001, f = 0.999999999; // 1e-9

    // When
    i32 res = math::fcmp(a, b);
    i32 res2 = math::fcmp(a, c);
    i32 res3 = math::fcmp(a, d);
    i32 res4 = math::fcmp(a, e);
    i32 res5 = math::fcmp(a, f);

    // Then
    Assertions::assertEquals(0, res);
    Assertions::assertEquals(-1, res2);
    Assertions::assertEquals(1, res3);
    Assertions::assertEquals(0, res4);
    Assertions::assertEquals(0, res5);
};

inline void test_math_utils() {
    UnitTestGroup group{"test_math_utils"};

    group.addTest("should_cal_gcd", should_cal_gcd);
    group.addTest("should_cal_lcm", should_cal_lcm);
    group.addTest("should_judge_is_prime", should_judge_is_prime);
    group.addTest("should_cal_pow", should_cal_pow);
    group.addTest("should_compare", should_compare);

    group.startAll();
}

} // namespace my::test::test_math_utils

#endif // TEST_MATH_UTILS_HPP