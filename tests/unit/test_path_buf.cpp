#include "test_path_buf.hpp"
#include "path_buf.hpp"
#include "ricky_test.hpp"

#include <string>

namespace my::test::test_path_buf {

void test_is_absolute_and_relative() {
    fs::PathBuf abs_win(R"(C:\Windows)");
    fs::PathBuf abs_posix("/usr/bin");
    fs::PathBuf rel("usr/bin");
    fs::PathBuf empty("");

    Assertions::assert_true(abs_win.is_absolute());
    Assertions::assert_true(abs_posix.is_absolute());
    Assertions::assert_true(rel.is_relative());
    Assertions::assert_false(empty.is_absolute());
}

void test_join_push_pop_parent() {
    fs::PathBuf base("a/b");
    auto joined = base.join("c.txt");
    Assertions::assert_equals(std::string("c.txt"), joined.file_name().into_string());

    base.push("c.txt");
    Assertions::assert_equals(std::string("c.txt"), base.file_name().into_string());

    base.pop();
    Assertions::assert_equals(std::string("b"), base.file_name().into_string());

    auto parent = fs::PathBuf("a/b/c").parent();
    Assertions::assert_equals(std::string("b"), parent.file_name().into_string());
}

void test_file_name_stem_extension() {
    fs::PathBuf p("dir/file.tar.gz");
    Assertions::assert_equals(std::string("file.tar.gz"), p.file_name().into_string());
    Assertions::assert_equals(std::string("file.tar"), p.file_stem().into_string());
    Assertions::assert_equals(std::string("gz"), p.extension().into_string());
}

void test_set_extension() {
    fs::PathBuf p1("dir/file.tar.gz");
    Assertions::assert_true(p1.set_extension("txt"));
    Assertions::assert_equals(std::string("file.tar.txt"), p1.file_name().into_string());

    fs::PathBuf p2("dir/file.tar.gz");
    Assertions::assert_true(p2.set_extension(""));
    Assertions::assert_equals(std::string("file.tar"), p2.file_name().into_string());

    fs::PathBuf p3("dir/file.tar.gz");
    Assertions::assert_true(p3.set_extension(".log"));
    Assertions::assert_equals(std::string("file.tar.log"), p3.file_name().into_string());
}

void test_pop_edge_cases() {
    fs::PathBuf empty;
    Assertions::assert_false(empty.pop());

    fs::PathBuf root("/");
    Assertions::assert_true(root.pop());
    Assertions::assert_true(root.is_empty());
}

GROUP_NAME("test_path_buf");
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(test_is_absolute_and_relative),
    UNIT_TEST_ITEM(test_join_push_pop_parent),
    UNIT_TEST_ITEM(test_file_name_stem_extension),
    UNIT_TEST_ITEM(test_set_extension),
    UNIT_TEST_ITEM(test_pop_edge_cases));

} // namespace my::test::test_path_buf
