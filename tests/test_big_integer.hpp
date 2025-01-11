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
    Assertions::assertEquals(9, bi.size());
    Assertions::assertEquals("0"_cs, bi2.__str__());
    Assertions::assertEquals(1, bi2.size());
    Assertions::assertEquals("-123456789"_cs, bi3.__str__());
    Assertions::assertEquals(9, bi3.size());
};

auto should_construct_with_c_string = []() {
    // Given
    CString num = "123456789"_cs;
    CString num2 = "0"_cs;
    CString num3 = "-123456789"_cs;
    CString num4 = "-00123456789"_cs;

    // When
    math::BigInteger bi = num;
    math::BigInteger bi2 = num2;
    math::BigInteger bi3 = num3;
    math::BigInteger bi4 = num4;

    // Then
    Assertions::assertEquals("123456789"_cs, bi.__str__());
    Assertions::assertEquals(9, bi.size());
    Assertions::assertEquals("0"_cs, bi2.__str__());
    Assertions::assertEquals(1, bi2.size());
    Assertions::assertEquals("-123456789"_cs, bi3.__str__());
    Assertions::assertEquals(9, bi3.size());
    Assertions::assertEquals("-123456789"_cs, bi4.__str__());
    Assertions::assertEquals(9, bi4.size());
};

void test_big_integer() {
    UnitTestGroup group{"test_big_integer"};

    group.addTest("should_construct_with_i64", should_construct_with_i64);
    group.addTest("should_construct_with_c_string", should_construct_with_c_string);

    group.startAll();
}

} // namespace my::test::test_big_integer

#endif // TEST_BIG_INTEGER_HPP