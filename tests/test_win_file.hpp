#ifndef TEST_WIN_FILE_HPP
#define TEST_WIN_FILE_HPP

#include "ricky_test.hpp"
#include "filesystem.hpp"
#include "win/file.hpp"

namespace my::test::test_win_file {

static constexpr auto CLASS_PATH = R"(F:\Develop\cpp\ricky-cpp\tests\resources)";

auto should_write_win_file = []() {
    // Given
    auto path = fs::win::join(CLASS_PATH, "test1.txt");
    fs::win::File file{path, "w"};

    // When
    file.write("hello file.");

    // Then
    Assertions::assertTrue(fs::win::exists(path));
    Assertions::assertEquals(11, file.fileSize());

    file.close();
    Assertions::assertEquals("hello file."_cs, fs::win::File{path, "r"}.read());

    // Final
    fs::win::remove(path);
};

auto should_fail_to_construct_if_mode_invalid = []() {
    // Given
    auto path = fs::win::join(CLASS_PATH, "test1.txt");

    // When & Then
    Assertions::assertThrows("Invalid value x, that only support [w, r, a]", [path]() {
        fs::win::File(path, "x");
    });
};

auto should_append_win_file = []() {
    // Given
    auto path = fs::win::join(CLASS_PATH, "test2.txt");
    fs::win::File file{path, "a"};

    // When
    file.write("hello1 ");
    file.write("hello2");

    // Then
    Assertions::assertEquals(13, file.fileSize());

    file.close();
    Assertions::assertEquals("hello1 hello2"_cs, fs::win::File{path, "r"}.read());

    // Final
    fs::win::remove(path);
};

auto test_win_file() {
    UnitTestGroup group{"test_win_file"};

    group.addTest("should_write_win_file", should_write_win_file);
    group.addTest("should_fail_to_construct_if_mode_invalid", should_fail_to_construct_if_mode_invalid);
    group.addTest("should_append_win_file", should_append_win_file);

    group.startAll();
}

} // namespace my::test::test_win_file

#endif // TEST_WIN_FILE_HPP