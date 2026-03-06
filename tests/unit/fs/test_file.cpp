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

void remove_if_exists(str::StringView path) {
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
    auto content = file.read_all();

    // Then
    Assertions::assert_true(content.find("Huffman Coding"_sv).is_some());
}

void test_create_write_and_read() {
    // Given
    auto path = make_res_path("fs_file_tmp_write.txt");
    auto path_cstr = path.as_cstr();
    remove_if_exists(str::StringView(path_cstr.data(), path_cstr.length()));
    const char data[] = "file write test";

    // When
    auto file = fs::File::create(path_cstr.data());
    Assertions::assert_true(file.is_open());
    auto written = file.write(data, sizeof(data) - 1);
    file.flush();
    file.close();

    // Then
    Assertions::assert_equals(static_cast<usize>(sizeof(data) - 1), written);
    auto content = fs::File::open(path_cstr.data()).read_all();
    std::string content_std(reinterpret_cast<const char*>(content.as_bytes()), content.len());
    Assertions::assert_equals(std::string(data), content_std);

    // Final
    plat::fs::remove(str::StringView(path_cstr.data(), path_cstr.length()));
}

void test_append() {
    // Given
    auto path = make_res_path("fs_file_tmp_append.txt");
    auto path_cstr = path.as_cstr();
    remove_if_exists(str::StringView(path_cstr.data(), path_cstr.length()));

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
    auto content = fs::File::open(path_cstr.data()).read_all();
    std::string content_std(reinterpret_cast<const char*>(content.as_bytes()), content.len());
    Assertions::assert_equals(std::string("ab"), content_std);

    // Final
    plat::fs::remove(str::StringView(path_cstr.data(), path_cstr.length()));
}

void should_throw_when_handle_invalid() {
    // Given
    auto path = make_res_path("fs_file_tmp_invalid.txt");
    auto path_cstr = path.as_cstr();
    remove_if_exists(str::StringView(path_cstr.data(), path_cstr.length()));
    auto file = fs::File::create(path_cstr.data());
    file.close();
    CString expected_msg = CString("Invalid file handle");

    // When & Then
    Assertions::assert_throws<Exception>(expected_msg, [&]() {
        file.read_all();
    });

    // Final
    plat::fs::remove(str::StringView(path_cstr.data(), path_cstr.length()));
}

GROUP_NAME("test_file");
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(test_open_and_read_all),
    UNIT_TEST_ITEM(test_create_write_and_read),
    UNIT_TEST_ITEM(test_append),
    UNIT_TEST_ITEM(should_throw_when_handle_invalid));

} // namespace my::test::test_file
