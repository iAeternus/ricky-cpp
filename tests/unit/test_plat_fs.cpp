#include "test_plat_fs.hpp"
#include "fs.hpp"
#include "ricky_test.hpp"

namespace my::test::test_plat_fs {

namespace {

const std::string& repo_root() {
    static const std::string root = []() {
        std::string file = __FILE__;
        const char* win_suffix = "\\tests\\unit\\test_plat_fs.cpp";
        const char* posix_suffix = "/tests/unit/test_plat_fs.cpp";
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

util::String unit_file() {
    return plat::fs::join(repo_root().c_str(), R"(tests\unit\test_win_file_utils.hpp)");
}

util::String tests_dir() {
    return plat::fs::join(repo_root().c_str(), R"(tests)");
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

void test_exists() {
    // Given
    const auto path_not_exists = ".\\aaa.txt";
    auto filepath = unit_file();
    auto dir_path = tests_dir();

    // When
    bool res = plat::fs::exists(filepath.__str__().data());
    bool res2 = plat::fs::exists(dir_path.__str__().data());
    bool res3 = plat::fs::exists(path_not_exists);

    // Then
    Assertions::assert_true(res);
    Assertions::assert_true(res2);
    Assertions::assert_false(res3);
}

void test_is_file() {
    auto filepath = unit_file();
    auto dir_path = tests_dir();

    Assertions::assert_true(plat::fs::is_file(filepath.__str__().data()));
    Assertions::assert_false(plat::fs::is_file(dir_path.__str__().data()));
}

void test_is_dir() {
    auto filepath = unit_file();
    auto dir_path = tests_dir();

    Assertions::assert_false(plat::fs::is_dir(filepath.__str__().data()));
    Assertions::assert_true(plat::fs::is_dir(dir_path.__str__().data()));
}

void test_mkdir() {
    // Given
    auto path = make_res_path("text").into_string();

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
    auto res_path = res_dir().__str__();
    expected_msg += CStringView(res_path.data());

    // When & Then
    Assertions::assert_throws<Exception>(expected_msg, [&]() {
        plat::fs::mkdir(res_path.data(), false, false);
    });
}

void should_fail_to_mkdir_if_path_not_found() {
    // Given
    auto path = make_res_path(R"(tmp1\tmp2)").into_string();
    CString expected_msg = CString(R"(Failed to create directory: )");
    expected_msg += CStringView(path.c_str());

    // When & Then
    Assertions::assert_throws<Exception>(expected_msg, [&]() {
        plat::fs::mkdir(path.data());
    });
}

void test_remove() {
    // Given
    auto path = make_res_path("text").into_string();
    plat::fs::mkdir(path.data());

    // When
    plat::fs::remove(path.data());

    // Then
    Assertions::assert_false(plat::fs::exists(path.data()));
}

void should_fail_to_remove_if_file_or_dir_not_found() {
    // Given
    auto path = make_res_path(R"(tmp1\tmp2)").into_string();
    CString expected_msg = CString(R"(File or directory not found: )");
    expected_msg += CStringView(path.c_str());

    // When & Then
    Assertions::assert_throws<Exception>(expected_msg, [&]() {
        plat::fs::remove(path.data());
    });
}

void test_join() {
    auto res_path = res_dir().__str__();
    auto joined = plat::fs::join(res_path.data(), "text.txt").into_string();
    Assertions::assert_true(joined.find(R"(tests\resources\text.txt)") != std::string::npos);

    auto joined2 = plat::fs::join(res_path.data(), "").into_string();
    Assertions::assert_equals(std::string(res_path.data()), joined2);
}

void test_listdir() {
    // Given
    auto tmp_dir = make_res_path("plat_fs_tmp_dir");
    remove_if_exists(tmp_dir.__str__().data());
    plat::fs::mkdir(tmp_dir.__str__().data());

    // When
    auto res_path = res_dir().__str__();
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

    plat::fs::remove(tmp_dir.__str__().data());
}

void test_open_rb() {
    // Given
    auto path = make_res_path("text.txt");

    // When
    auto* handle = plat::fs::open(path.__str__().data(), plat::fs::OpenMode::ReadBinary);

    // Then
    Assertions::assert_not_null(handle);

    // Final
    plat::fs::close(handle);
}

void test_open_wb() {
    // Given
    auto tmp_file = make_res_path("plat_fs_tmp_open.txt");
    remove_if_exists(tmp_file.__str__().data());

    // When
    auto* handle = plat::fs::open(tmp_file.__str__().data(), plat::fs::OpenMode::WriteBinary);

    // Then
    Assertions::assert_not_null(handle);

    // Final
    plat::fs::close(handle);
    plat::fs::remove(tmp_file.__str__().data());
}

void test_read_all() {
    // Given
    auto path = make_res_path("text.txt");

    // When
    auto content = plat::fs::read_all(path.__str__().data()).into_string();

    // Then
    Assertions::assert_true(content.find("Huffman Coding") != std::string::npos);
}

void test_write() {
    // Given
    auto tmp_file = make_res_path("plat_fs_tmp_write.txt");
    remove_if_exists(tmp_file.__str__().data());

    // When
    auto* handle = plat::fs::open(tmp_file.__str__().data(), plat::fs::OpenMode::WriteBinary);
    const char data[] = "plat fs write test";
    const auto written = plat::fs::write(handle, data, sizeof(data) - 1);
    Assertions::assert_equals(static_cast<usize>(sizeof(data) - 1), written);
    plat::fs::close(handle);

    // Then
    auto content = plat::fs::read_all(tmp_file.__str__().data()).into_string();
    Assertions::assert_equals(std::string(data), content);
    plat::fs::remove(tmp_file.__str__().data());
}

void test_flush() {
    // Given
    auto tmp_file = make_res_path("plat_fs_tmp_flush.txt");
    remove_if_exists(tmp_file.__str__().data());

    // When
    auto* handle = plat::fs::open(tmp_file.__str__().data(), plat::fs::OpenMode::WriteBinary);
    const char data[] = "flush";
    plat::fs::write(handle, data, sizeof(data) - 1);
    plat::fs::flush(handle);
    plat::fs::close(handle);

    // Then
    auto content = plat::fs::read_all(tmp_file.__str__().data()).into_string();
    Assertions::assert_equals(std::string(data), content);
    plat::fs::remove(tmp_file.__str__().data());
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
