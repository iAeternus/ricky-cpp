#include "test_path_buf.hpp"
#include "path_buf.hpp"
#include "ricky_test.hpp"

#include <string>

namespace my::test::test_path_buf {

namespace {

std::string to_std(const my::str::String<>& s) {
    auto view = s.as_str();
    return std::string(reinterpret_cast<const char*>(view.as_bytes()), view.len());
}

} // namespace

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
    Assertions::assert_equals(std::string("c.txt"), to_std(joined.file_name()));

    base.push("c.txt");
    Assertions::assert_equals(std::string("c.txt"), to_std(base.file_name()));

    base.pop();
    Assertions::assert_equals(std::string("b"), to_std(base.file_name()));

    auto parent = fs::PathBuf("a/b/c").parent();
    Assertions::assert_equals(std::string("b"), to_std(parent.file_name()));
}

void test_file_name_stem_extension() {
    fs::PathBuf p("dir/file.tar.gz");
    Assertions::assert_equals(std::string("file.tar.gz"), to_std(p.file_name()));
    Assertions::assert_equals(std::string("file.tar"), to_std(p.file_stem()));
    Assertions::assert_equals(std::string("gz"), to_std(p.extension()));
}

void test_set_extension() {
    fs::PathBuf p1("dir/file.tar.gz");
    Assertions::assert_true(p1.set_extension("txt"));
    Assertions::assert_equals(std::string("file.tar.txt"), to_std(p1.file_name()));

    fs::PathBuf p2("dir/file.tar.gz");
    Assertions::assert_true(p2.set_extension(""));
    Assertions::assert_equals(std::string("file.tar"), to_std(p2.file_name()));

    fs::PathBuf p3("dir/file.tar.gz");
    Assertions::assert_true(p3.set_extension(".log"));
    Assertions::assert_equals(std::string("file.tar.log"), to_std(p3.file_name()));
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
