#ifndef TEST_WIN_FILE_UTILS_HPP
#define TEST_WIN_FILE_UTILS_HPP

#include "my_types.hpp"

namespace my::test::test_win_file_utils {
void should_judge_exists();
void should_judge_is_file();
void should_judge_is_dir();
void should_mkdir_and_remove();
void should_fail_to_mkdir_if_dir_already_exists();
void should_fail_to_mkdir_if_path_not_found();
void should_fail_to_remove_if_file_or_dir_not_found();
void should_join();
void should_list_dir();
void test_win_file_utils();
} // namespace my::test::test_win_file_utils

#endif // TEST_WIN_FILE_UTILS_HPP