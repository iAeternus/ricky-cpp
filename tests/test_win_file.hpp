#ifndef TEST_WIN_FILE_HPP
#define TEST_WIN_FILE_HPP

#include "ricky_test.hpp"
#include "filesystem.hpp"

namespace my::test::test_win_file {

static constexpr const char* CLASS_PATH = "F:\\develop\\ricky-cpp\\tests\\resources";

auto should_write_win_file = []() {
    // // Given
    const char* path = fs::win::join(CLASS_PATH, "test1.txt");
    fs::win::File file{path, "w"};

    // When
    file.write("hello file.");

    // Then
    Assertions::assertTrue(fs::win::exists(path));
    Assertions::assertEquals(11LL, file.fileSize());

    file.close();
    Assertions::assertEquals("hello file."_cs, fs::win::File{path, "r"}.read());

    // Final
    fs::win::remove(path);
};

auto should_append_win_file = []() {
    // Given
    const char* path = fs::win::join(CLASS_PATH, "test2.txt");
    fs::win::File file{path, "a"};

    // When
    file.write("hello1 ");
    file.write("hello2");

    // Then
    Assertions::assertEquals(13LL, file.fileSize());

    file.close();
    Assertions::assertEquals("hello1 hello2"_cs, fs::win::File{path, "r"}.read());

    // Final
    fs::win::remove(path);
};

void test_win_file() {
    UnitTestGroup group{"test_win_file"};

    group.addTest("should_write_win_file", should_write_win_file);
    group.addTest("should_append_win_file", should_append_win_file);

    group.startAll();
}

} // namespace my::test::test_win_file

#endif // TEST_WIN_FILE_HPP