#include "test_file.hpp"
#include "file.hpp"
#include "ricky_test.hpp"

namespace my::test::test_file {

namespace {

const std::string& repo_root() {
    static const std::string root = []() {
        std::string file = __FILE__;
        const char* win_suffix = "\\tests\\unit\\test_file.cpp";
        const char* posix_suffix = "/tests/unit/test_file.cpp";
        auto pos = file.find(win_suffix);
        if (pos == std::string::npos) {
            pos = file.find(posix_suffix);
        }
        if (pos == std::string::npos) {
            return std::string(".");
        }
        return file.substr(0, pos);
    }();
    return root;
}

util::String res_dir() {
    return plat::fs::join(repo_root().c_str(), R"(tests\resources)");
}

util::String make_res_path(const char* leaf) {
    auto base = res_dir().__str__();
    return plat::fs::join(base.data(), leaf);
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
    auto file = fs::File::open(path.__str__().data());
    auto content = file.read_all().into_string();

    // Then
    Assertions::assert_true(content.find("Huffman Coding") != std::string::npos);
}

void test_create_write_and_read() {
    // Given
    auto path = make_res_path("fs_file_tmp_write.txt").into_string();
    remove_if_exists(path.c_str());
    const char data[] = "file write test";

    // When
    auto file = fs::File::create(path.c_str());
    Assertions::assert_true(file.is_open());
    auto written = file.write(data, sizeof(data) - 1);
    file.flush();
    file.close();

    // Then
    Assertions::assert_equals(static_cast<usize>(sizeof(data) - 1), written);
    auto content = fs::File::open(path.c_str()).read_all().into_string();
    Assertions::assert_equals(std::string(data), content);

    // Final
    plat::fs::remove(path.c_str());
}

void test_append() {
    // Given
    auto path = make_res_path("fs_file_tmp_append.txt").into_string();
    remove_if_exists(path.c_str());

    // When
    {
        auto file = fs::File::create(path.c_str());
        file.write("a", 1);
    }
    {
        auto file = fs::File::append(path.c_str());
        file.write("b", 1);
    }

    // Then
    auto content = fs::File::open(path.c_str()).read_all().into_string();
    Assertions::assert_equals(std::string("ab"), content);

    // Final
    plat::fs::remove(path.c_str());
}

void should_throw_when_handle_invalid() {
    // Given
    auto path = make_res_path("fs_file_tmp_invalid.txt").into_string();
    remove_if_exists(path.c_str());
    auto file = fs::File::create(path.c_str());
    file.close();
    CString expected_msg = CString("Invalid file handle");

    // When & Then
    Assertions::assert_throws<Exception>(expected_msg, [&]() {
        file.read_all();
    });

    // Final
    plat::fs::remove(path.c_str());
}

GROUP_NAME("test_file");
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(test_open_and_read_all),
    UNIT_TEST_ITEM(test_create_write_and_read),
    UNIT_TEST_ITEM(test_append),
    UNIT_TEST_ITEM(should_throw_when_handle_invalid));

} // namespace my::test::test_file
