#ifndef TEST_BIG_INTEGER_HPP
#define TEST_BIG_INTEGER_HPP

#include "ricky_test.hpp"
#include "big_integer.hpp"

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
    Assertions::assertEquals(9, bi.size());
    Assertions::assertEquals("0"_cs, bi2.__str__());
    Assertions::assertEquals(1, bi2.size());
    Assertions::assertEquals("-123456789"_cs, bi3.__str__());
    Assertions::assertEquals(9, bi3.size());
};

auto should_construct_with_c_string = []() {
    // Given
    CString num = "123456789012345678901234567890";
    CString num2 = "0";
    CString num3 = "-123456789012345678901234567890";
    CString num4 = "-00123456789012345678901234567890";

    // When
    math::BigInteger bi = num;
    math::BigInteger bi2 = num2;
    math::BigInteger bi3 = num3;
    math::BigInteger bi4 = num4;

    // Then
    Assertions::assertEquals("123456789012345678901234567890"_cs, bi.__str__());
    Assertions::assertEquals(30, bi.size());
    Assertions::assertEquals("0"_cs, bi2.__str__());
    Assertions::assertEquals(1, bi2.size());
    Assertions::assertEquals("-123456789012345678901234567890"_cs, bi3.__str__());
    Assertions::assertEquals(30, bi3.size());
    Assertions::assertEquals("-123456789012345678901234567890"_cs, bi4.__str__());
    Assertions::assertEquals(30, bi4.size());
};

auto should_fail_to_construct_if_str_invalid = []() {
    Assertions::assertThrows("Invalid string", []() {
        math::BigInteger bi = nullptr;
    });
};

auto should_fail_to_construct_if_invalid_char_in_str = []() {
    Assertions::assertThrows("Invalid character in string", []() {
        math::BigInteger bi = "123a456";
    });
};

auto should_judge_odd = []() {
    // Given
    math::BigInteger bi = "123456789012345678901234567890";
    math::BigInteger bi2 = "123456789012345678901234567891";
    math::BigInteger bi3 = math::BigInteger::ZERO;

    // When
    bool res = bi.is_odd();
    bool res2 = bi2.is_odd();
    bool res3 = bi3.is_odd();

    // Then
    Assertions::assertFalse(res);
    Assertions::assertTrue(res2);
    Assertions::assertFalse(res3);
};

auto should_left_shift = []() {
    // Given
    math::BigInteger bi = "123456789012345678901234567890"_cs;

    // When
    auto res = bi.left_shift(10);
    auto res2 = bi.left_shift(0);

    // Then
    Assertions::assertEquals("1234567890123456789012345678900000000000"_cs, res.__str__());
    Assertions::assertEquals("123456789012345678901234567890"_cs, res2.__str__());
};

auto should_right_shift = []() {
    // Given
    math::BigInteger bi = "123456789012345678901234567890";
    math::BigInteger bi2 = "-123456789012345678901234567890";

    // When
    auto res = bi.right_shift(10);
    auto res2 = bi.right_shift(30);
    auto res3 = bi.right_shift(31);
    auto res4 = bi2.right_shift(30);

    // Then
    Assertions::assertEquals("12345678901234567890"_cs, res.__str__());
    Assertions::assertEquals(math::BigInteger::ZERO, res2);
    Assertions::assertEquals(math::BigInteger::ZERO, res3);
    Assertions::assertEquals(math::BigInteger::ZERO, res4);
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
    auto res2 = bi / math::BigInteger::TEN.pow(27);
    bi /= bi2;
    bi2 /= 2;

    // Then
    Assertions::assertEquals("8"_cs, res.__str__());
    Assertions::assertEquals("123"_cs, res2.__str__());
    Assertions::assertEquals(math::BigInteger::ZERO, bi);
    Assertions::assertEquals("493827160549382716054938271605"_cs, bi2.__str__());
};

auto should_fail_to_divide_if_divide_by_zero = []() {
    Assertions::assertThrows("/ by zero", []() {
        math::BigInteger(123) / math::BigInteger::ZERO;
    });
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

auto should_power = []() {
    // Given
    math::BigInteger base = "123456789012345678901234567890";

    // When
    auto res = base ^ 10;
    auto res2 = base ^ 0;
    auto res3 = base.pow(10);
    auto res4 = base.pow(0);

    // Then
    Assertions::assertEquals("822526259969628839104253165869933624624768975718986341753117113191672345101686635234711078432787527087114699126238380568851450669625883238384735536304145587136095844229774592556217075848515269880288897142287955821529180675549369033497201746908666410370342866279796500763077997366010000000000"_cs, res.__str__());
    Assertions::assertEquals(math::BigInteger::ONE, res2);
    Assertions::assertEquals("822526259969628839104253165869933624624768975718986341753117113191672345101686635234711078432787527087114699126238380568851450669625883238384735536304145587136095844229774592556217075848515269880288897142287955821529180675549369033497201746908666410370342866279796500763077997366010000000000"_cs, res3.__str__());
    Assertions::assertEquals(math::BigInteger::ONE, res4);
};

auto should_slice = []() {
    // Given
    math::BigInteger bi = "123456789012345678901234567890";
    math::BigInteger bi2 = "-123456789012345678901234567890";

    // When
    auto res = bi.slice(1, 1);
    auto res2 = bi.slice(11, 20);
    auto res3 = bi.slice(1);
    auto res4 = bi.slice(11);

    auto res5 = bi2.slice(1, 1);
    auto res6 = bi2.slice(11, 20);
    auto res7 = bi2.slice(1);
    auto res8 = bi2.slice(11);

    // Then
    Assertions::assertEquals("0"_cs, res.__str__());
    Assertions::assertEquals("1234567890"_cs, res2.__str__());
    Assertions::assertEquals("123456789012345678901234567890"_cs, res3.__str__());
    Assertions::assertEquals("12345678901234567890"_cs, res4.__str__());
    Assertions::assertEquals("0"_cs, res5.__str__());
    Assertions::assertEquals("-1234567890"_cs, res6.__str__());
    Assertions::assertEquals("-123456789012345678901234567890"_cs, res7.__str__());
    Assertions::assertEquals("-12345678901234567890"_cs, res8.__str__());
};

auto should_logical = []() {
    // Given
    math::BigInteger bi = "123456789012345678901234567890";
    math::BigInteger bi2 = "987654321098765432109876543210";

    // When
    bool res = bi && bi2;
    bool res2 = math::BigInteger::ZERO && bi2;
    bool res3 = bi || bi2;
    bool res4 = math::BigInteger::ZERO || bi2;
    bool res5 = !bi;
    bool res6 = !math::BigInteger::ZERO;

    // Then
    Assertions::assertTrue(res);
    Assertions::assertFalse(res2);
    Assertions::assertTrue(res3);
    Assertions::assertTrue(res4);
    Assertions::assertFalse(res5);
    Assertions::assertTrue(res6);
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

auto test_big_integer() {
    UnitTestGroup group{"test_big_integer"};

    group.addTest("should_construct_with_i64", should_construct_with_i64);
    group.addTest("should_construct_with_c_string", should_construct_with_c_string);
    group.addTest("should_fail_to_construct_if_str_invalid", should_fail_to_construct_if_str_invalid);
    group.addTest("should_fail_to_construct_if_invalid_char_in_str", should_fail_to_construct_if_invalid_char_in_str);
    group.addTest("should_judge_odd", should_judge_odd);
    group.addTest("should_left_shift", should_left_shift);
    group.addTest("should_right_shift", should_right_shift);
    group.addTest("should_add", should_add);
    group.addTest("should_subtract", should_subtract);
    group.addTest("should_multiply", should_multiply);
    group.addTest("should_divide", should_divide);
    group.addTest("should_fail_to_divide_if_divide_by_zero", should_fail_to_divide_if_divide_by_zero);
    group.addTest("should_modulus", should_modulus);
    group.addTest("should_power", should_power);
    group.addTest("should_slice", should_slice);
    group.addTest("should_logical", should_logical);
    group.addTest("should_compare", should_compare);

    group.startAll();
}

} // namespace my::test::test_big_integer

#endif // TEST_BIG_INTEGER_HPP