#ifndef TEST_WIN_FILE_HPP
#define TEST_WIN_FILE_HPP

#include "my_types.hpp"

namespace my::test::test_win_file {
void should_write_win_file();
void should_fail_to_construct_if_mode_invalid();
void should_append_win_file();
void test_win_file();
} // namespace my::test::test_win_file

#endif // TEST_WIN_FILE_HPP