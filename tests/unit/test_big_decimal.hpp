#ifndef TEST_BIG_DECIMAL_HPP
#define TEST_BIG_DECIMAL_HPP

#include "my_types.hpp"

namespace my::test::test_big_decimal {
void should_construct();
void test_abs();
void test_add();
void test_sub();
void test_mul();
void test_div();
void test_scale();
void test_round();
void test_rounding_mode();
void test_move_point();
void test_strip_trailing_zeros();
void test_sqrt();
void test_precision();
void test_big_decimal();
} // namespace my::test::test_big_decimal

#endif // TEST_BIG_DECIMAL_HPP