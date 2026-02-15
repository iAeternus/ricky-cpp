#ifndef TEST_NUM_BASE_HPP
#define TEST_NUM_BASE_HPP

namespace my::test::test_num_base {

void basic_binary_to_decimal();
void conversion_with_width();
void hex_to_decimal();
void decimal_to_hex();
void negative_conversion();
void negative_with_width();
void zero_conversion();
void empty_string();
void large_number();
void llmin_conversion();
void invalid_base();
void invalid_characters();
void digit_exceeds_base();
void binary_to_octal();
void decimal_to_base36();
void base8_to_base16();
void hex_with_width();
void overflow_test();
void only_minus_sign();
void mixed_case();

} // namespace my::test::test_num_base

#endif // TEST_NUM_BASE_HPP