#ifndef TEST_STRING_ALGORITHM_HPP
#define TEST_STRING_ALGORITHM_HPP

namespace my::test::test_string_algorithm {

// twoway_find
void test_twoway_find_basic();
void test_twoway_find_no_match();
void test_twoway_find_empty_pattern();
void test_twoway_find_pattern_longer_than_text();
void test_twoway_find_single_char();
void test_twoway_find_repeated_chars();
void test_twoway_find_periodic_pattern();
void test_twoway_find_utf8_bytes();
void test_twoway_find_match_at_end();

// kmp_find
void test_kmp_find_basic();
void test_kmp_find_no_match();
void test_kmp_find_empty();
void test_kmp_find_single_char();
void test_kmp_find_repeated_chars();

// kmp_find_all
void test_kmp_find_all_no_match();
void test_kmp_find_all_basic();
void test_kmp_find_all_multiple();
void test_kmp_find_all_overlapping();
void test_kmp_find_all_empty();
void test_kmp_find_all_single_char();

} // namespace my::test::test_string_algorithm

#endif // TEST_STRING_ALGORITHM_HPP
