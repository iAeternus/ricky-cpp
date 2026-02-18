#include "test_file.hpp"
#include "file.hpp"
#include "path_buf.hpp"
#include "ricky_test.hpp"

namespace my::test::test_file {

namespace {

const fs::PathBuf& repo_root() {
    static const fs::PathBuf root = []() {
        std::string file = __FILE__;
        const char* win_suffix = "\\tests\\unit\\fs\\test_file.cpp";
        const char* posix_suffix = "/tests/unit/fs/test_file.cpp";
        auto pos = file.find(win_suffix);
        if (pos == std::string::npos) {
            pos = file.find(posix_suffix);
        }
        if (pos == std::string::npos) {
            return fs::PathBuf(".");
        }
        return fs::PathBuf(file.substr(0, pos).c_str());
    }();
    return root;
}

fs::PathBuf res_dir() {
    return repo_root().join(R"(tests\resources)");
}

fs::PathBuf make_res_path(const char* leaf) {
    return res_dir().join(leaf);
}

void remove_if_exists(const char* path) {
    if (plat::fs::exists(path)) {
        plat::fs::remove(path, true);
    }
}

} // namespace

void test_open_and_read_all() {
    // Given
    auto path = make_res_path("text.txt");

    // When
    auto file = fs::File::open(path);
    auto content = file.read_all().into_string();

    // Then
    Assertions::assert_true(content.find("Huffman Coding") != std::string::npos);
}

void test_create_write_and_read() {
    // Given
    auto path = make_res_path("fs_file_tmp_write.txt");
    auto path_cstr = path.as_cstr();
    remove_if_exists(path_cstr.data());
    const char data[] = "file write test";

    // When
    auto file = fs::File::create(path_cstr.data());
    Assertions::assert_true(file.is_open());
    auto written = file.write(data, sizeof(data) - 1);
    file.flush();
    file.close();

    // Then
    Assertions::assert_equals(static_cast<usize>(sizeof(data) - 1), written);
    auto content = fs::File::open(path_cstr.data()).read_all().into_string();
    Assertions::assert_equals(std::string(data), content);

    // Final
    plat::fs::remove(path_cstr.data());
}

void test_append() {
    // Given
    auto path = make_res_path("fs_file_tmp_append.txt");
    auto path_cstr = path.as_cstr();
    remove_if_exists(path_cstr.data());

    // When
    {
        auto file = fs::File::create(path_cstr.data());
        file.write("a", 1);
    }
    {
        auto file = fs::File::append(path_cstr.data());
        file.write("b", 1);
    }

    // Then
    auto content = fs::File::open(path_cstr.data()).read_all().into_string();
    Assertions::assert_equals(std::string("ab"), content);

    // Final
    plat::fs::remove(path_cstr.data());
}

void should_throw_when_handle_invalid() {
    // Given
    auto path = make_res_path("fs_file_tmp_invalid.txt");
    auto path_cstr = path.as_cstr();
    remove_if_exists(path_cstr.data());
    auto file = fs::File::create(path_cstr.data());
    file.close();
    CString expected_msg = CString("Invalid file handle");

    // When & Then
    Assertions::assert_throws<Exception>(expected_msg, [&]() {
        file.read_all();
    });

    // Final
    plat::fs::remove(path_cstr.data());
}

GROUP_NAME("test_file");
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(test_open_and_read_all),
    UNIT_TEST_ITEM(test_create_write_and_read),
    UNIT_TEST_ITEM(test_append),
    UNIT_TEST_ITEM(should_throw_when_handle_invalid));

} // namespace my::test::test_file
