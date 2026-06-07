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

// New tests for added features
void test_from_numeric();
void test_operator_add();
void test_operator_mul();
void test_match_indices();
void test_string_trim_start_end();
void test_trim_matches();
void test_join();
void test_match_brackets();
void test_remove_all();
void test_swap();
void test_push_format();
void test_push_n();
void test_shrink_to_fit();
void test_slice_negative();
void test_split_max_split();
void test_string_iterators();
void test_at_out_of_range();
void test_comparison_operators();
void test_copy_and_move();
void test_split_pattern();
void test_string_view_trim();
void test_string_view_match_indices();
void test_string_view_split_max();
void test_string_view_iterators();
void test_string_view_to_numeric();
void test_retain();
void test_string_trim_no_whitespace();

} // namespace my::test::test_str_string

#endif // TEST_STR_STRING_HPP
