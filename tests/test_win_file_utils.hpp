#ifndef TEST_WIN_FILE_UTILS_HPP
#define TEST_WIN_FILE_UTILS_HPP

#include "Printer.hpp"
#include "UnitTest.hpp"
#include "Assertions.hpp"
#include "filesystem.hpp"
#include "win/file_utils.hpp"

namespace my::test::test_win_file_utils {

static constexpr auto CLASS_PATH = R"(F:\Develop\cpp\ricky-cpp\tests\resources)";
inline auto filepath = R"(F:\Develop\cpp\ricky-cpp\tests\test_win_file_utils.hpp)";
inline auto dir_path = R"(F:\Develop\cpp\ricky-cpp\tests)";

fn should_judge_exists = []() {
    // Given
    const auto path_not_exists = ".\\aaa.txt";

    // When
    bool res = fs::win::exists(filepath);
    bool res2 = fs::win::exists(dir_path);
    bool res3 = fs::win::exists(path_not_exists);

    // Then
    Assertions::assertTrue(res);
    Assertions::assertTrue(res2);
    Assertions::assertFalse(res3);
};

fn should_judge_is_file = []() {
    // When
    bool res = fs::win::isfile(filepath);
    bool res2 = fs::win::isfile(dir_path);

    // Then
    Assertions::assertTrue(res);
    Assertions::assertFalse(res2);
};

fn should_judge_is_dir = []() {
    // When
    bool res = fs::win::isdir(filepath);
    bool res2 = fs::win::isdir(dir_path);

    // Then
    Assertions::assertFalse(res);
    Assertions::assertTrue(res2);
};

fn should_mkdir_and_remove = []() {
    // Given
    auto path = fs::win::join(CLASS_PATH, "test");

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

fn should_fail_to_mkdir_if_dir_already_exists = []() {
    Assertions::assertThrows(R"(directory already exists: F:\Develop\tmp\ricky-cpp\tests\resources)", []() {
        fs::win::mkdir(CLASS_PATH, false);
    });
};

fn should_fail_to_mkdir_if_path_not_found = []() {
    Assertions::assertThrows(R"(path not found: F:\Develop\tmp\ricky-cpp\tests\resources\tmp1\tmp2)", []() {
        fs::win::mkdir(fs::win::join(CLASS_PATH, "tmp1\\tmp2"));
    });
};

fn should_fail_to_remove_if_file_or_dir_not_found = []() {
    Assertions::assertThrows(R"(file or directory not found in F:\Develop\cpp\ricky-cpp\tests\resources\tmp1\tmp2)", []() {
        fs::win::remove(fs::win::join(CLASS_PATH, "tmp1\\tmp2"));
    });
};

fn should_join = []() {
    // Given
    const char* path = "C:";
    const char* path2 = "C:\\";
    const char* path3 = "test\\";

    // When
    auto res = fs::win::join(path, path3);
    auto res2 = fs::win::join(path2, path3);

    // Then
    Assertions::assertEquals("C:\\test\\"_cs, res);
    Assertions::assertEquals("C:\\test\\"_cs, res2);
};

fn should_list_dir = []() {
    // When
    auto filenames = fs::win::listdir(dir_path);

    // Then
    for (const auto& filename : filenames) {
        io::println(filename);
    }
};

fn test_win_file_utils() {
    UnitTestGroup group{"test_win_file_utils"};

    group.addTest("should_judge_exists", should_judge_exists);
    group.addTest("should_judge_is_file", should_judge_is_file);
    group.addTest("should_judge_is_dir", should_judge_is_dir);
    group.addTest("should_mkdir_and_remove", should_mkdir_and_remove);
    // group.addTest("should_fail_to_mkdir_if_dir_already_exists", should_fail_to_mkdir_if_dir_already_exists);
    // group.addTest("should_fail_to_mkdir_if_path_not_found", should_fail_to_mkdir_if_path_not_found);
    group.addTest("should_fail_to_remove_if_file_or_dir_not_found", should_fail_to_remove_if_file_or_dir_not_found);
    group.addTest("should_join", should_join);
    group.addTest("should_list_dir", should_list_dir);

    group.startAll();
}

} // namespace my::test::test_win_file_utils

#endif // TEST_WIN_FILE_UTILS_HPP