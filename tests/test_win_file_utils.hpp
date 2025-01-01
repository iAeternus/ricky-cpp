#ifndef TEST_WIN_FILE_UTILS_HPP
#define TEST_WIN_FILE_UTILS_HPP

#include "ricky_test.hpp"
#include "win/file_utils.hpp"

namespace my::test::test_win_file_utils {

const char* filepath = "F:\\develop\\ricky-cpp\\tests\\test_win_file_utils.hpp";
const char* dirpath = "F:\\develop\\ricky-cpp\\tests";

auto should_judge_exists = []() {
    // Given
    const char* pathnotexists = "F:\\develop\\ricky-cpp\\tests\\aaa.txt";

    // When
    bool res = fs::win::exists(filepath);
    bool res2 = fs::win::exists(dirpath);
    bool res3 = fs::win::exists(pathnotexists);

    // Then
    Assertions::assertTrue(res);
    Assertions::assertTrue(res2);
    Assertions::assertFalse(res3);
};

auto should_judge_is_file = []() {
    // When
    bool res = fs::win::isfile(filepath);
    bool res2 = fs::win::isfile(dirpath);

    // Then
    Assertions::assertTrue(res);
    Assertions::assertFalse(res2);
};

auto should_judge_is_dir = []() {
    // When
    bool res = fs::win::isdir(filepath);
    bool res2 = fs::win::isdir(dirpath);

    // Then
    Assertions::assertFalse(res);
    Assertions::assertTrue(res2);
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
    Assertions::assertEquals("C:\\test\\"_cs, res);
    Assertions::assertEquals("C:\\test\\"_cs, res2);
};

auto should_list_dir = []() {
    // When
    auto filenames = fs::win::listdir(dirpath);

    // Then
    for(const auto& filename : filenames) {
        io::print(filename);
    }
};

void test_win_file_utils() {
    UnitTestGroup group{"test_win_file_utils"};

    group.addTest("should_judge_exists", should_judge_exists);
    group.addTest("should_judge_is_file", should_judge_is_file);
    group.addTest("should_judge_is_dir", should_judge_is_dir);
    group.addTest("should_join", should_join);
    group.addTest("should_list_dir", should_list_dir);

    group.startAll();
}

} // namespace my::test::test_win_file_utils

#endif // TEST_WIN_FILE_UTILS_HPP