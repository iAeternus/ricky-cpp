#ifndef TEST_WIN_FILE_UTILS_HPP
#define TEST_WIN_FILE_UTILS_HPP

#include "ricky_test.hpp"
#include "filesystem.hpp"

namespace my::test::test_win_file_utils {

static constexpr const char* CLASS_PATH = R"(F:\develop\ricky-cpp\tests\resources)";
const char* filepath = R"(F:\develop\ricky-cpp\tests\test_win_file_utils.hpp)";
const char* dir_path = R"(F:\develop\ricky-cpp\tests)";

auto should_judge_exists = []() {
    // Given
    const char* path_not_exists = R"(F:\develop\ricky-cpp\tests\aaa.txt)";

    // When
    bool res = fs::win::exists(filepath);
    bool res2 = fs::win::exists(dir_path);
    bool res3 = fs::win::exists(path_not_exists);

    // Then
    Assertions::assertTrue(res);
    Assertions::assertTrue(res2);
    Assertions::assertFalse(res3);
};

auto should_judge_is_file = []() {
    // When
    bool res = fs::win::isfile(filepath);
    bool res2 = fs::win::isfile(dir_path);

    // Then
    Assertions::assertTrue(res);
    Assertions::assertFalse(res2);
};

auto should_judge_is_dir = []() {
    // When
    bool res = fs::win::isdir(filepath);
    bool res2 = fs::win::isdir(dir_path);

    // Then
    Assertions::assertFalse(res);
    Assertions::assertTrue(res2);
};

auto should_mkdir_and_remove = []() {
    // Given
    const char* path = fs::win::join(CLASS_PATH, "test");

    // When
    fs::win::mkdir(path);

    // Then
    Assertions::assertTrue(fs::win::exists(path));

    // When
    fs::win::mkdir(path, true);

    // Then
    Assertions::assertTrue(fs::win::exists(path));

    // When
    fs::win::remove(path);

    // Then
    Assertions::assertFalse(fs::win::exists(path));
};

auto should_join = []() {
    // Given
    const char* path = "C:";
    const char* path2 = "C:\\";
    const char* path3 = "test\\";

    // When
    auto res = fs::win::join(path, path3);
    auto res2 = fs::win::join(path2, path3);

    // Then
    Assertions::assertEquals(R"(C:\test\)"_cs, res);
    Assertions::assertEquals(R"(C:\test\)"_cs, res2);
};

auto should_list_dir = []() {
    // When
    auto filenames = fs::win::listdir(dir_path);

    // Then
    for (const auto& filename : filenames) {
        io::println(filename);
    }
};

void test_win_file_utils() {
    UnitTestGroup group{"test_win_file_utils"};

    group.addTest("should_judge_exists", should_judge_exists);
    group.addTest("should_judge_is_file", should_judge_is_file);
    group.addTest("should_judge_is_dir", should_judge_is_dir);
    group.addTest("should_mkdir_and_remove", should_mkdir_and_remove);
    group.addTest("should_join", should_join);
    group.addTest("should_list_dir", should_list_dir);

    group.startAll();
}

} // namespace my::test::test_win_file_utils

#endif // TEST_WIN_FILE_UTILS_HPP