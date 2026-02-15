#include "test_win_file_utils.hpp"
#include "printer.hpp"
#include "filesystem.hpp"
#include "win/file_utils.hpp"
#include "ricky_test.hpp"

namespace my::test::test_win_file_utils {

static constexpr auto CLASS_PATH = R"(F:\Develop\cpp\ricky-cpp\tests\resources)";
inline auto filepath = R"(F:\Develop\cpp\ricky-cpp\tests\unit\test_win_file_utils.hpp)";
inline auto dir_path = R"(F:\Develop\cpp\ricky-cpp\tests)";

void should_judge_exists() {
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
}

void should_judge_is_file() {
    // When
    bool res = fs::win::isfile(filepath);
    bool res2 = fs::win::isfile(dir_path);

    // Then
    Assertions::assertTrue(res);
    Assertions::assertFalse(res2);
}

void should_judge_is_dir() {
    // When
    bool res = fs::win::isdir(filepath);
    bool res2 = fs::win::isdir(dir_path);

    // Then
    Assertions::assertFalse(res);
    Assertions::assertTrue(res2);
}

void should_mkdir_and_remove() {
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
}

void should_fail_to_mkdir_if_dir_already_exists() {
    Assertions::assertThrows(R"(directory already exists: F:\Develop\tmp\ricky-cpp\tests\resources)", []() {
        fs::win::mkdir(CLASS_PATH, false);
    });
}

void should_fail_to_mkdir_if_path_not_found() {
    Assertions::assertThrows(R"(path not found: F:\Develop\tmp\ricky-cpp\tests\resources\tmp1\tmp2)", []() {
        fs::win::mkdir(fs::win::join(CLASS_PATH, "tmp1\\tmp2"));
    });
}

void should_fail_to_remove_if_file_or_dir_not_found() {
    Assertions::assertThrows(R"(File or directory not found in F:\Develop\cpp\ricky-cpp\tests\resources\tmp1\tmp2)", []() {
        fs::win::remove(fs::win::join(CLASS_PATH, "tmp1\\tmp2"));
    });
}

void should_join() {
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
}

void should_list_dir() {
    // When
    auto filenames = fs::win::listdir(dir_path);

    // Then
    for (const auto& filename : filenames) {
        io::println(filename);
    }
}

GROUP_NAME("test_win_file_utils")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(should_judge_exists),
    UNIT_TEST_ITEM(should_judge_is_file),
    UNIT_TEST_ITEM(should_judge_is_dir),
    UNIT_TEST_ITEM(should_mkdir_and_remove),
    UNIT_TEST_ITEM(should_fail_to_remove_if_file_or_dir_not_found),
    UNIT_TEST_ITEM(should_join),
    UNIT_TEST_ITEM(should_list_dir))

} // namespace my::test::test_win_file_utils
