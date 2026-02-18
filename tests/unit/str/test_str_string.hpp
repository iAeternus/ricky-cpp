#ifndef TEST_STR_STRING_HPP
#define TEST_STR_STRING_HPP

namespace my::test::test_str_string {

void test_basic_len_and_bytes();
void test_empty_basic();
void test_bytes_range_methods();
void test_chars_range_methods();
void test_chars_bytes_utf8();
void test_push_pop();
void test_push_pop_edge_cases();
void test_find_basic();
void test_find_edge_cases();
void test_contains_starts_ends_edge();
void test_strip_prefix_suffix_edge();
void test_split_basic();
void test_split_empty_pattern();
void test_split_whitespace_edge_cases();
void test_lines_edge_cases();
void test_replace_basic();
void test_replace_edge_cases();
void test_trim_edge_cases();
void test_case_convert_and_into_bytes();
void test_case_convert_non_ascii();
void test_into_bytes_edge_cases();
void test_reserve_capacity();

} // namespace my::test::test_str_string

#endif // TEST_STR_STRING_HPP
