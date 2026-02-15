#ifndef TEST_STRING_HPP
#define TEST_STRING_HPP

#include "my_types.hpp"

namespace my::test::test_string {
void should_construct();
void should_add();
void should_mul();
void should_slice();
void should_find();
void should_find_all();
void should_judge_starts_with();
void should_judge_ends_with();
void should_get_upper();
void should_get_lower();
void should_trim();
void should_replace();
void should_maintain_encoding();
void should_join_iterator();
void should_match_parentheses();
void should_fail_match_if_str_invalid();
} // namespace my::test::test_string

#endif // TEST_STRING_HPP