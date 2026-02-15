#ifndef TEST_COMPLEX_HPP
#define TEST_COMPLEX_HPP

#include "my_types.hpp"

namespace my::test::test_complex {
void should_construct();
void should_fail_to_parse_if_invalid_format();
void should_calc_modulus();
void should_calc_arg();
void should_add();
void should_subtract();
void should_multiply();
void should_divide();
void test_complex();
} // namespace my::test::test_complex

#endif // TEST_COMPLEX_HPP