#ifndef TEST_BIG_INTEGER_HPP
#define TEST_BIG_INTEGER_HPP

#include "my_types.hpp"

namespace my::test::test_big_integer {
void should_construct_with_i64();
void should_construct_with_c_string();
void should_fail_to_construct_if_str_invalid();
void should_fail_to_construct_if_invalid_char_in_str();
void should_judge_odd();
void should_left_shift();
void should_right_shift();
void should_add();
void should_subtract();
void should_multiply();
void should_divide();
void should_fail_to_divide_if_divide_by_zero();
void should_modulus();
void should_power();
void should_slice();
void should_logical();
void should_compare();
void test_big_integer();
} // namespace my::test::test_big_integer

#endif // TEST_BIG_INTEGER_HPP