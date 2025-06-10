#ifndef TEST_BIG_DECIMAL_HPP
#define TEST_BIG_DECIMAL_HPP

#include "UnitTest.hpp"
#include "Assertions.hpp"
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

// fn test_div = []() {
//     // Given
//     math::BigDecimal bd = "3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679";
//     math::BigDecimal bd2 = "100.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100";

//     // When
//     auto res = bd + bd2;

//     // Then
//     Assertions::assertEquals("-97.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170579"_cs, res.__str__());
// };

fn test_big_decimal() {
    UnitTestGroup group{"test_big_decimal"};

    group.addTest("should_construct", should_construct);
    group.addTest("test_abs", test_abs);
    group.addTest("test_add", test_add);
    group.addTest("test_sub", test_sub);
    group.addTest("test_mul", test_mul);

    group.startAll();
}

} // namespace my::test::test_big_decimal

#endif // TEST_BIG_DECIMAL_HPP