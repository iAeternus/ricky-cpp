#ifndef TEST_PLAT_FS_HPP
#define TEST_PLAT_FS_HPP

namespace my::test::test_plat_fs {

void test_exists();
void test_is_file();
void test_is_dir();
void test_mkdir();
void should_fail_to_mkdir_if_dir_already_exists();
void should_fail_to_mkdir_if_path_not_found();
void test_remove();
void should_fail_to_remove_if_file_or_dir_not_found();
void test_join();
void test_listdir();
void test_open_rb();
void test_open_wb();
void test_read_all();
void test_write();
void test_flush();

} // namespace my::test::test_plat_fs

#endif // TEST_PLAT_FS_HPP