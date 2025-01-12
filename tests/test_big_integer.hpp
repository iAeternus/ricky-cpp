#ifndef TEST_BIG_INTEGER_HPP
#define TEST_BIG_INTEGER_HPP

#include "ricky_test.hpp"
#include "BigInteger.hpp"

namespace my::test::test_big_integer {

auto should_construct_with_i64 = []() {
    // Given
    i64 num = 123456789;
    i64 num2 = 0;
    i64 num3 = -123456789;

    // When
    math::BigInteger bi = num;
    math::BigInteger bi2 = num2;
    math::BigInteger bi3 = num3;

    // Then
    Assertions::assertEquals("123456789"_cs, bi.__str__());
    Assertions::assertEquals(9LL, bi.size());
    Assertions::assertEquals("0"_cs, bi2.__str__());
    Assertions::assertEquals(1LL, bi2.size());
    Assertions::assertEquals("-123456789"_cs, bi3.__str__());
    Assertions::assertEquals(9LL, bi3.size());
};

auto should_construct_with_c_string = []() {
    // Given
    CString num = "123456789012345678901234567890"_cs;
    CString num2 = "0"_cs;
    CString num3 = "-123456789012345678901234567890"_cs;
    CString num4 = "-00123456789012345678901234567890"_cs;

    // When
    math::BigInteger bi = num;
    math::BigInteger bi2 = num2;
    math::BigInteger bi3 = num3;
    math::BigInteger bi4 = num4;

    // Then
    Assertions::assertEquals("123456789012345678901234567890"_cs, bi.__str__());
    Assertions::assertEquals(30LL, bi.size());
    Assertions::assertEquals("0"_cs, bi2.__str__());
    Assertions::assertEquals(1LL, bi2.size());
    Assertions::assertEquals("-123456789012345678901234567890"_cs, bi3.__str__());
    Assertions::assertEquals(30LL, bi3.size());
    Assertions::assertEquals("-123456789012345678901234567890"_cs, bi4.__str__());
    Assertions::assertEquals(30LL, bi4.size());
};

auto should_add = []() {
    // Given
    math::BigInteger bi = "123456789012345678901234567890";
    math::BigInteger bi2 = "987654321098765432109876543210";

    // When
    auto res = bi + bi2;

    // Then
    Assertions::assertEquals("1111111110111111111011111111100"_cs, res.__str__());

    // When
    bi2 += bi;

    // Then
    Assertions::assertEquals(res, bi2);

    // When
    auto res2 = +bi;

    // Then
    Assertions::assertEquals("123456789012345678901234567890"_cs, res2.__str__());

    // When
    auto res3 = bi++;
    auto res4 = ++bi;

    // Then
    Assertions::assertEquals("123456789012345678901234567890"_cs, res3.__str__());
    Assertions::assertEquals("123456789012345678901234567892"_cs, res4.__str__());
};

auto should_subtract = []() {
    // Given
    math::BigInteger bi = "123456789012345678901234567890";
    math::BigInteger bi2 = "987654321098765432109876543210";

    // When
    auto res = bi - bi2;

    // Then
    Assertions::assertEquals("-864197532086419753208641975320"_cs, res.__str__());

    // When
    bi2 -= bi;

    // Then
    Assertions::assertEquals("864197532086419753208641975320"_cs, bi2.__str__());

    // When
    auto res2 = -bi;

    // Then
    Assertions::assertEquals("-123456789012345678901234567890"_cs, res2.__str__());

    // When
    auto res3 = bi--;
    auto res4 = --bi;

    // Then
    Assertions::assertEquals("123456789012345678901234567890"_cs, res3.__str__());
    Assertions::assertEquals("123456789012345678901234567888"_cs, res4.__str__());
};

auto should_multiply = []() {
    // Given
    math::BigInteger bi = "123456789012345678901234567890";
    math::BigInteger bi2 = "987654321098765432109876543210";

    // When
    auto res = bi * bi2;
    bi *= bi2;

    // Then
    Assertions::assertEquals("121932631137021795226185032733622923332237463801111263526900"_cs, res.__str__());
    Assertions::assertEquals(res, bi);
};

auto should_divide = []() {
    // Given
    math::BigInteger bi = "123456789012345678901234567890";
    math::BigInteger bi2 = "987654321098765432109876543210";

    // When
    auto res = bi2 / bi;
    bi /= bi2;
    bi2 /= 2;

    // Then
    Assertions::assertEquals("8"_cs, res.__str__());
    Assertions::assertEquals(math::BigInteger::ZERO, bi);
    Assertions::assertEquals("493827160549382716054938271605"_cs, bi2.__str__());
};

auto should_modulus = []() {
    // Given
    math::BigInteger bi = "123456789012345678901234567890";
    math::BigInteger bi2 = "987654321098765432109876543210";

    // When
    auto res = bi % bi2;
    bi2 %= bi;

    // Then
    Assertions::assertEquals("123456789012345678901234567890"_cs, res.__str__());
    Assertions::assertEquals("9000000000900000000090"_cs, bi2.__str__());
};

auto should_compare = []() {
    // Given
    math::BigInteger bi = "123456789012345678901234567890";
    math::BigInteger bi2 = "987654321098765432109876543210";

    // When
    bool res = bi < bi2;
    bool res2 = bi <= bi2;
    bool res3 = bi > bi2;
    bool res4 = bi >= bi2;
    bool res5 = bi == bi2;
    bool res6 = bi != bi2;

    // Then
    Assertions::assertTrue(res);
    Assertions::assertTrue(res2);
    Assertions::assertFalse(res3);
    Assertions::assertFalse(res4);
    Assertions::assertFalse(res5);
    Assertions::assertTrue(res6);
};

void test_big_integer() {
    UnitTestGroup group{"test_big_integer"};

    group.addTest("should_construct_with_i64", should_construct_with_i64);
    group.addTest("should_construct_with_c_string", should_construct_with_c_string);
    group.addTest("should_add", should_add);
    group.addTest("should_subtract", should_subtract);
    group.addTest("should_multiply", should_multiply);
    group.addTest("should_divide", should_divide);
    group.addTest("should_modulus", should_modulus);
    group.addTest("should_compare", should_compare);

    group.startAll();
}

} // namespace my::test::test_big_integer

#endif // TEST_BIG_INTEGER_HPP