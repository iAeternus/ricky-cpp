#ifndef TEST_OPTION_AND_RESULT_HPP
#define TEST_OPTION_AND_RESULT_HPP

#include "my_types.hpp"

namespace my::test::test_option_and_result {
void test_option_basic();
void test_option_copy_move();
void test_option_unwrap_or();
void test_option_map();
void test_option_and_then();
void test_result_basic();
void test_result_copy_move();
void test_result_map();
void test_result_map_err();
void test_result_and_then();
void test_result_unwrap_or();
void test_option_and_result();
} // namespace my::test::test_option_and_result

#endif // TEST_OPTION_AND_RESULT_HPP