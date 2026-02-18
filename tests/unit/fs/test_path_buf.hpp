#ifndef TEST_PATH_BUF_HPP
#define TEST_PATH_BUF_HPP

namespace my::test::test_path_buf {

void test_is_absolute_and_relative();
void test_join_push_pop_parent();
void test_file_name_stem_extension();
void test_set_extension();
void test_pop_edge_cases();

} // namespace my::test::test_path_buf

#endif // TEST_PATH_BUF_HPP
