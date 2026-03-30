#include "test_plat_fs.hpp"
#include "fs.hpp"
#include "path_buf.hpp"
#include "ricky_test.hpp"

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
    return repo_root().join("tests/resources");
}

fs::PathBuf unit_file() {
    return repo_root().join(R"(tests\CMakeLists.txt)");
}

fs::PathBuf tests_dir() {
    return repo_root().join(R"(tests)");
}

fs::PathBuf make_res_path(const char* leaf) {
    return res_dir().join(leaf);
}

str::StringView sv(const CString& s) {
    return str::StringView(s.data(), s.length());
}

std::string to_std(const str::String<>& s) {
    auto view = s.as_str();
    return std::string(reinterpret_cast<const char*>(view.as_bytes()), view.len());
}

bool equals_sv(str::StringView a, const char* b) {
    const auto b_len = b ? static_cast<usize>(std::strlen(b)) : 0;
    if (a.len() != b_len) {
        return false;
    }
    return b_len == 0 || std::memcmp(a.as_bytes(), b, b_len) == 0;
}

void remove_if_exists(str::StringView path) {
    if (plat::fs::exists(path)) {
        plat::fs::remove(path, true);
    }
}

} // namespace

void test_exists() {
    // Given
    const auto path_not_exists = ".\\aaa.txt"_sv;
    auto filepath = unit_file();
    auto dir_path = tests_dir();

    // When
    bool res = plat::fs::exists(sv(filepath.as_cstr()));
    bool res2 = plat::fs::exists(sv(dir_path.as_cstr()));
    bool res3 = plat::fs::exists(path_not_exists);

    // Then
    Assertions::assert_true(res);
    Assertions::assert_true(res2);
    Assertions::assert_false(res3);
}

void test_is_file() {
    auto filepath = unit_file();
    auto dir_path = tests_dir();

    Assertions::assert_true(plat::fs::is_file(sv(filepath.as_cstr())));
    Assertions::assert_false(plat::fs::is_file(sv(dir_path.as_cstr())));
}

void test_is_dir() {
    auto filepath = unit_file();
    auto dir_path = tests_dir();

    Assertions::assert_false(plat::fs::is_dir(sv(filepath.as_cstr())));
    Assertions::assert_true(plat::fs::is_dir(sv(dir_path.as_cstr())));
}

void test_mkdir() {
    // Given
    auto path = make_res_path("text").as_cstr();

    // When
    plat::fs::mkdir(sv(path));

    // Then
    Assertions::assert_true(plat::fs::exists(sv(path)));

    // When
    plat::fs::mkdir(sv(path), false, true);

    // Then
    Assertions::assert_true(plat::fs::exists(sv(path)));
}

void should_fail_to_mkdir_if_dir_already_exists() {
    // Given
    CString expected_msg = CString(R"(Directory already exists: )");
    auto res_path = res_dir().as_cstr();
    expected_msg += CStringView(res_path.data());

    // When & Then
    Assertions::assert_throws<Exception>(expected_msg, [&]() {
        plat::fs::mkdir(sv(res_path), false, false);
    });
}

void should_fail_to_mkdir_if_path_not_found() {
    // Given
    auto path = make_res_path(R"(tmp1\tmp2)").as_cstr();
    CString expected_msg = CString(R"(Failed to create directory: )");
    expected_msg += CStringView(path.data());

    // When & Then
    Assertions::assert_throws<Exception>(expected_msg, [&]() {
        plat::fs::mkdir(sv(path));
    });
}

void test_remove() {
    // Given
    auto path = make_res_path("text").as_cstr();
    plat::fs::mkdir(sv(path));

    // When
    plat::fs::remove(sv(path));

    // Then
    Assertions::assert_false(plat::fs::exists(sv(path)));
}

void should_fail_to_remove_if_file_or_dir_not_found() {
    // Given
    auto path = make_res_path(R"(tmp1\tmp2)").as_cstr();
    CString expected_msg = CString(R"(File or directory not found: )");
    expected_msg += CStringView(path.data());

    // When & Then
    Assertions::assert_throws<Exception>(expected_msg, [&]() {
        plat::fs::remove(sv(path));
    });
}

void test_join() {
    auto res_path = res_dir().as_cstr();
    auto joined = plat::fs::join(sv(res_path), "text.txt"_sv);
    auto joined_str = to_std(joined);
    auto pos1 = joined_str.find("tests/resources/text.txt");
    auto pos2 = joined_str.find("tests\\resources\\text.txt");
    Assertions::assert_true(pos1 != std::string::npos || pos2 != std::string::npos);

    auto joined2 = plat::fs::join(sv(res_path), ""_sv);
    Assertions::assert_equals(sv(res_path), joined2.as_str());
}

void test_listdir() {
    // Given
    auto tmp_dir = make_res_path("plat_fs_tmp_dir");
    auto tmp_dir_cstr = tmp_dir.as_cstr();
    remove_if_exists(sv(tmp_dir_cstr));
    plat::fs::mkdir(sv(tmp_dir_cstr));

    // When
    auto res_path = res_dir().as_cstr();
    auto entries = plat::fs::listdir(sv(res_path));
    bool found_text = false;
    bool found_tmp_dir = false;
    for (const auto& entry : entries) {
        if (equals_sv(entry.name.as_str(), "text.txt")) {
            found_text = entry.is_file;
        }
        if (equals_sv(entry.name.as_str(), "plat_fs_tmp_dir")) {
            found_tmp_dir = entry.is_dir;
        }
    }

    // Then
    Assertions::assert_true(found_text);
    Assertions::assert_true(found_tmp_dir);

    plat::fs::remove(sv(tmp_dir_cstr));
}

void test_open_rb() {
    // Given
    auto path = make_res_path("text.txt").as_cstr();

    // When
    auto* handle = plat::fs::open(sv(path), plat::fs::OpenMode::ReadBinary);

    // Then
    Assertions::assert_not_null(handle);

    // Final
    plat::fs::close(handle);
}

void test_open_wb() {
    // Given
    auto tmp_file = make_res_path("plat_fs_tmp_open.txt").as_cstr();
    remove_if_exists(sv(tmp_file));

    // When
    auto* handle = plat::fs::open(sv(tmp_file), plat::fs::OpenMode::WriteBinary);

    // Then
    Assertions::assert_not_null(handle);

    // Final
    plat::fs::close(handle);
    plat::fs::remove(sv(tmp_file));
}

void test_read_all() {
    // Given
    auto path = make_res_path("text.txt").as_cstr();

    // When
    auto content = to_std(plat::fs::read_all(sv(path)));

    // Then
    Assertions::assert_true(content.find("Huffman Coding") != std::string::npos);
}

void test_write() {
    // Given
    auto tmp_file = make_res_path("plat_fs_tmp_write.txt").as_cstr();
    remove_if_exists(sv(tmp_file));

    // When
    auto* handle = plat::fs::open(sv(tmp_file), plat::fs::OpenMode::WriteBinary);
    const char data[] = "plat fs write test";
    const auto written = plat::fs::write(handle, str::StringView(data, sizeof(data) - 1), sizeof(data) - 1);
    Assertions::assert_equals(static_cast<usize>(sizeof(data) - 1), written);
    plat::fs::close(handle);

    // Then
    auto content = to_std(plat::fs::read_all(sv(tmp_file)));
    Assertions::assert_equals(std::string(data), content);
    plat::fs::remove(sv(tmp_file));
}

void test_flush() {
    // Given
    auto tmp_file = make_res_path("plat_fs_tmp_flush.txt").as_cstr();
    remove_if_exists(sv(tmp_file));

    // When
    auto* handle = plat::fs::open(sv(tmp_file), plat::fs::OpenMode::WriteBinary);
    const char data[] = "flush";
    plat::fs::write(handle, str::StringView(data, sizeof(data) - 1), sizeof(data) - 1);
    plat::fs::flush(handle);
    plat::fs::close(handle);

    // Then
    auto content = to_std(plat::fs::read_all(sv(tmp_file)));
    Assertions::assert_equals(std::string(data), content);
    plat::fs::remove(sv(tmp_file));
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
