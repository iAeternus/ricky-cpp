#include "test_plat_fs.hpp"
#include "fs.hpp"
#include "path_buf.hpp"
#include "ricky_test.hpp"

#include <string>

namespace my::test::test_plat_fs {

namespace {

const fs::PathBuf& repo_root() {
    static const fs::PathBuf root = []() {
        std::string file = __FILE__;
        const char* win_suffix = "\\tests\\unit\\plat\\test_plat_fs.cpp";
        const char* posix_suffix = "/tests/unit/plat/test_plat_fs.cpp";
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

fs::PathBuf unit_file() {
    return repo_root().join(R"(tests\unit\test_win_file_utils.hpp)");
}

fs::PathBuf tests_dir() {
    return repo_root().join(R"(tests)");
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

void test_exists() {
    // Given
    const auto path_not_exists = ".\\aaa.txt";
    auto filepath = unit_file();
    auto dir_path = tests_dir();

    // When
    bool res = plat::fs::exists(filepath.as_cstr().data());
    bool res2 = plat::fs::exists(dir_path.as_cstr().data());
    bool res3 = plat::fs::exists(path_not_exists);

    // Then
    Assertions::assert_true(res);
    Assertions::assert_true(res2);
    Assertions::assert_false(res3);
}

void test_is_file() {
    auto filepath = unit_file();
    auto dir_path = tests_dir();

    Assertions::assert_true(plat::fs::is_file(filepath.as_cstr().data()));
    Assertions::assert_false(plat::fs::is_file(dir_path.as_cstr().data()));
}

void test_is_dir() {
    auto filepath = unit_file();
    auto dir_path = tests_dir();

    Assertions::assert_false(plat::fs::is_dir(filepath.as_cstr().data()));
    Assertions::assert_true(plat::fs::is_dir(dir_path.as_cstr().data()));
}

void test_mkdir() {
    // Given
    auto path = make_res_path("text").as_cstr();

    // When
    plat::fs::mkdir(path.data());

    // Then
    Assertions::assert_true(plat::fs::exists(path.data()));

    // When
    plat::fs::mkdir(path.data(), false, true);

    // Then
    Assertions::assert_true(plat::fs::exists(path.data()));
}

void should_fail_to_mkdir_if_dir_already_exists() {
    // Given
    CString expected_msg = CString(R"(Directory already exists: )");
    auto res_path = res_dir().as_cstr();
    expected_msg += CStringView(res_path.data());

    // When & Then
    Assertions::assert_throws<Exception>(expected_msg, [&]() {
        plat::fs::mkdir(res_path.data(), false, false);
    });
}

void should_fail_to_mkdir_if_path_not_found() {
    // Given
    auto path = make_res_path(R"(tmp1\tmp2)").as_cstr();
    CString expected_msg = CString(R"(Failed to create directory: )");
    expected_msg += CStringView(path.data());

    // When & Then
    Assertions::assert_throws<Exception>(expected_msg, [&]() {
        plat::fs::mkdir(path.data());
    });
}

void test_remove() {
    // Given
    auto path = make_res_path("text").as_cstr();
    plat::fs::mkdir(path.data());

    // When
    plat::fs::remove(path.data());

    // Then
    Assertions::assert_false(plat::fs::exists(path.data()));
}

void should_fail_to_remove_if_file_or_dir_not_found() {
    // Given
    auto path = make_res_path(R"(tmp1\tmp2)").as_cstr();
    CString expected_msg = CString(R"(File or directory not found: )");
    expected_msg += CStringView(path.data());

    // When & Then
    Assertions::assert_throws<Exception>(expected_msg, [&]() {
        plat::fs::remove(path.data());
    });
}

void test_join() {
    auto res_path = res_dir().as_cstr();
    auto joined = plat::fs::join(res_path.data(), "text.txt").into_string();
    Assertions::assert_true(joined.find(R"(tests\resources\text.txt)") != std::string::npos);

    auto joined2 = plat::fs::join(res_path.data(), "").into_string();
    Assertions::assert_equals(std::string(res_path.data()), joined2);
}

void test_listdir() {
    // Given
    auto tmp_dir = make_res_path("plat_fs_tmp_dir");
    auto tmp_dir_cstr = tmp_dir.as_cstr();
    remove_if_exists(tmp_dir_cstr.data());
    plat::fs::mkdir(tmp_dir_cstr.data());

    // When
    auto res_path = res_dir().as_cstr();
    auto entries = plat::fs::listdir(res_path.data());
    bool found_text = false;
    bool found_tmp_dir = false;
    for (const auto& entry : entries) {
        auto name = entry.name.__str__();
        if (name == "text.txt") {
            found_text = entry.is_file;
        }
        if (name == "plat_fs_tmp_dir") {
            found_tmp_dir = entry.is_dir;
        }
    }

    // Then
    Assertions::assert_true(found_text);
    Assertions::assert_true(found_tmp_dir);

    plat::fs::remove(tmp_dir_cstr.data());
}

void test_open_rb() {
    // Given
    auto path = make_res_path("text.txt").as_cstr();

    // When
    auto* handle = plat::fs::open(path.data(), plat::fs::OpenMode::ReadBinary);

    // Then
    Assertions::assert_not_null(handle);

    // Final
    plat::fs::close(handle);
}

void test_open_wb() {
    // Given
    auto tmp_file = make_res_path("plat_fs_tmp_open.txt").as_cstr();
    remove_if_exists(tmp_file.data());

    // When
    auto* handle = plat::fs::open(tmp_file.data(), plat::fs::OpenMode::WriteBinary);

    // Then
    Assertions::assert_not_null(handle);

    // Final
    plat::fs::close(handle);
    plat::fs::remove(tmp_file.data());
}

void test_read_all() {
    // Given
    auto path = make_res_path("text.txt").as_cstr();

    // When
    auto content = plat::fs::read_all(path.data()).into_string();

    // Then
    Assertions::assert_true(content.find("Huffman Coding") != std::string::npos);
}

void test_write() {
    // Given
    auto tmp_file = make_res_path("plat_fs_tmp_write.txt").as_cstr();
    remove_if_exists(tmp_file.data());

    // When
    auto* handle = plat::fs::open(tmp_file.data(), plat::fs::OpenMode::WriteBinary);
    const char data[] = "plat fs write test";
    const auto written = plat::fs::write(handle, data, sizeof(data) - 1);
    Assertions::assert_equals(static_cast<usize>(sizeof(data) - 1), written);
    plat::fs::close(handle);

    // Then
    auto content = plat::fs::read_all(tmp_file.data()).into_string();
    Assertions::assert_equals(std::string(data), content);
    plat::fs::remove(tmp_file.data());
}

void test_flush() {
    // Given
    auto tmp_file = make_res_path("plat_fs_tmp_flush.txt").as_cstr();
    remove_if_exists(tmp_file.data());

    // When
    auto* handle = plat::fs::open(tmp_file.data(), plat::fs::OpenMode::WriteBinary);
    const char data[] = "flush";
    plat::fs::write(handle, data, sizeof(data) - 1);
    plat::fs::flush(handle);
    plat::fs::close(handle);

    // Then
    auto content = plat::fs::read_all(tmp_file.data()).into_string();
    Assertions::assert_equals(std::string(data), content);
    plat::fs::remove(tmp_file.data());
}

GROUP_NAME("test_plat_fs");
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(test_exists),
    UNIT_TEST_ITEM(test_is_file),
    UNIT_TEST_ITEM(test_is_dir),
    UNIT_TEST_ITEM(test_mkdir),
    UNIT_TEST_ITEM(should_fail_to_mkdir_if_dir_already_exists),
    UNIT_TEST_ITEM(should_fail_to_mkdir_if_path_not_found),
    UNIT_TEST_ITEM(test_remove),
    UNIT_TEST_ITEM(should_fail_to_remove_if_file_or_dir_not_found),
    UNIT_TEST_ITEM(test_join),
    UNIT_TEST_ITEM(test_listdir),
    UNIT_TEST_ITEM(test_open_rb),
    UNIT_TEST_ITEM(test_open_wb),
    UNIT_TEST_ITEM(test_read_all),
    UNIT_TEST_ITEM(test_write),
    UNIT_TEST_ITEM(test_flush));

} // namespace my::test::test_plat_fs
