#ifndef TEST_BIG_DECIMAL_HPP
#define TEST_BIG_DECIMAL_HPP

#include "ricky_test.hpp"
#include "BigDecimal.hpp"

namespace my::test::test_big_decimal {

fn should_construct = []() {
    // Given
    math::BigDecimal bd("0.123456789012345678901234567890");
    math::BigDecimal bd2("-100.123456789012345678901234567890");
    math::BigDecimal bd3("0.00000000000000000");
    math::BigDecimal bd4 = -3;

    // When
    auto res = bd.__str__();
    auto res2 = bd2.__str__();
    auto res3 = bd3.__str__();
    auto res4 = bd4.__str__();

    // Then
    Assertions::assertEquals("0.123456789012345678901234567890"_cs, res);
    Assertions::assertEquals("-100.123456789012345678901234567890"_cs, res2);
    Assertions::assertEquals("0"_cs, res3);
    Assertions::assertEquals("-3"_cs, res4);
};

fn test_abs = []() {
    // Given
    math::BigDecimal bd("-3.14159265358979");
    math::BigDecimal bd2("0.0");

    // When
    auto res = bd.abs();
    auto res2 = bd2.abs();

    // Then
    Assertions::assertEquals("3.14159265358979"_cs, res.__str__());
    Assertions::assertEquals("0"_cs, res2.__str__());
};

fn test_add = []() {
    // Given
    math::BigDecimal bd = "3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679";
    math::BigDecimal bd2 = "100.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001";

    // When
    auto res = bd + bd2;
    bd += bd2;

    // Then
    Assertions::assertEquals("103.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170779"_cs, res.__str__());
    Assertions::assertEquals(res, bd);
};

fn test_sub = []() {
    // Given
    math::BigDecimal bd = "3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679";
    math::BigDecimal bd2 = "100.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001";

    // When
    auto res = bd - bd2;
    bd -= bd2;

    // Then
    Assertions::assertEquals("-96.8584073464102067615373566167204971158028306006248941790250554076921835937137910013719651746578829421"_cs, res.__str__());
    Assertions::assertEquals(res, bd);
};

fn test_mul = []() {
    // Given
    math::BigDecimal bd = "3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679";
    math::BigDecimal bd2 = "0.01";
    math::BigDecimal bd3 = "10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";

    // When
    auto res = bd * bd2;
    auto res2 = bd * bd3;
    bd *= bd2;

    // Then
    Assertions::assertEquals("0.031415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679"_cs, res.__str__());
    Assertions::assertEquals("31415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_cs, res2.__str__());
    Assertions::assertEquals(res, bd);
};

fn test_div = []() {
    // Given
    math::BigDecimal bd =   "3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679";
    math::BigDecimal bd2 = "-3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679";
    math::BigDecimal bd3 = "0.2";
    math::BigDecimal bd4 = "6";

    // When
    auto res = bd / bd2;
    auto res2 = bd / bd3;
    auto res3 = bd.divide(bd3, 2, math::DOWN);
    auto res4 = math::BigDecimal::ONE / bd4.scale(16, math::HALF_UP);

    // Then
    Assertions::assertEquals("-1.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"_cs, res.__str__());
    Assertions::assertEquals("15.7079632679489661923132169163975144209858469968755291048747229615390820314310449931401741267105853395"_cs, res2.__str__());
    Assertions::assertEquals("15.70"_cs, res3.__str__());
    Assertions::assertEquals("0.1666666666666667"_cs, res4.__str__());
};

fn test_scale = []() {
    // Given
    math::BigDecimal bd("123.456789");
    
    // When & Then
    Assertions::assertEquals("123.456789000"_cs, bd.scale(9, math::HALF_UP).__str__());
    Assertions::assertEquals("123.457"_cs, bd.scale(3, math::HALF_UP).__str__());
    Assertions::assertEquals("123.456"_cs, bd.scale(3, math::DOWN).__str__());
    Assertions::assertEquals("123"_cs, bd.scale(0, math::HALF_UP).__str__());
    Assertions::assertEquals("123"_cs, bd.scale(0, math::DOWN).__str__());
};

fn test_round = []() {
    math::BigDecimal bd("123.456789");
    
    // 测试有效数字舍入
    Assertions::assertEquals("123"_cs, bd.round(3, math::HALF_UP).__str__());
    Assertions::assertEquals("123.5"_cs, bd.round(4, math::HALF_UP).__str__());
    Assertions::assertEquals("123.46"_cs, bd.round(5, math::HALF_UP).__str__());
    Assertions::assertEquals("123.457"_cs, bd.round(6, math::HALF_UP).__str__());
    
    // 测试边界情况
    math::BigDecimal bd2("0.5");
    Assertions::assertEquals("1"_cs, bd2.round(1, math::HALF_UP).__str__());
    
    math::BigDecimal bd3("-0.5");
    Assertions::assertEquals("-1"_cs, bd3.round(1, math::HALF_UP).__str__());
    
    math::BigDecimal bd4("999.9");
    Assertions::assertEquals("1000"_cs, bd4.round(3, math::HALF_UP).__str__());
};

fn test_move_point = []() {
    // Given
    math::BigDecimal bd("123.456789");
    
    // When & Then
    Assertions::assertEquals("12345.6789"_cs, bd.move_point_right(2).__str__());
    Assertions::assertEquals("1.23456789"_cs, bd.move_point_left(2).__str__());
    Assertions::assertEquals("123456789"_cs, bd.move_point_right(6).__str__());
    Assertions::assertEquals("0.123456789"_cs, bd.move_point_left(3).__str__());
    
    // 测试负数
    math::BigDecimal neg_bd("-123.456789");
    Assertions::assertEquals("-12345.6789"_cs, neg_bd.move_point_right(2).__str__());
    Assertions::assertEquals("-0.123456789"_cs, neg_bd.move_point_left(3).__str__());
};

fn test_strip_trailing_zeros = []() {
    // Given
    math::BigDecimal bd("123.456000");
    math::BigDecimal bd2("123.000000");
    math::BigDecimal bd3("123.0");
    math::BigDecimal bd4("0.0");
    
    // When & Then
    Assertions::assertEquals("123.456"_cs, bd.strip_trailing_zeros().__str__());
    Assertions::assertEquals("123"_cs, bd2.strip_trailing_zeros().__str__());
    Assertions::assertEquals("123"_cs, bd3.strip_trailing_zeros().__str__());
    Assertions::assertEquals("0"_cs, bd4.strip_trailing_zeros().__str__());
};

fn test_sqrt = []() {
    // Given
    math::BigDecimal bd("2");
    math::BigDecimal bd2("0.25");
    math::BigDecimal bd3("100");
    
    // When & Then
    Assertions::assertEquals("1.4142135624"_cs, bd.sqrt(10).__str__());
    Assertions::assertEquals("0.5"_cs, bd2.sqrt(1).__str__());
    Assertions::assertEquals("10"_cs, bd3.sqrt(0).__str__());
    
    // 测试精度控制
    Assertions::assertEquals("1.41421356237309504880"_cs, bd.sqrt(20).__str__());
    
    // 测试负数
    Assertions::assertThrows("Cannot calculate square root of negative number", []() {
        math::BigDecimal(-1).sqrt();
    });
};

fn test_precision = []() {
    // Given
    math::BigDecimal bd("123.456789");
    math::BigDecimal bd2("0.000123456789");
    math::BigDecimal bd3("100.00");
    
    // When & Then
    Assertions::assertEquals(9u, bd.precision());
    Assertions::assertEquals(9u, bd2.precision());
    Assertions::assertEquals(3u, bd3.precision());
};

fn test_big_decimal() {
    UnitTestGroup group{"test_big_decimal"};

    group.addTest("should_construct", should_construct);
    group.addTest("test_abs", test_abs);
    group.addTest("test_add", test_add);
    group.addTest("test_sub", test_sub);
    group.addTest("test_mul", test_mul);
    group.addTest("test_div", test_div);
    group.addTest("test_scale", test_scale);
    group.addTest("test_round", test_round);
    group.addTest("test_move_point", test_move_point);
    group.addTest("test_strip_trailing_zeros", test_strip_trailing_zeros);
    group.addTest("test_sqrt", test_sqrt);
    group.addTest("test_precision", test_precision);

    group.startAll();
}

} // namespace my::test::test_big_decimal

#endif // TEST_BIG_DECIMAL_HPP