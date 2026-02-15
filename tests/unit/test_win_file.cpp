#include "unit/test_win_file.hpp"

#include "ricky_test.hpp"
#include "filesystem.hpp"
#include "win/file.hpp"

#include "test/test_registry.hpp"

namespace my::test::test_win_file {

static constexpr auto CLASS_PATH = R"(F:\Develop\cpp\ricky-cpp\tests\resources)";

void should_write_win_file() {
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
}

void should_fail_to_construct_if_mode_invalid() {
    // Given
    auto path = fs::win::join(CLASS_PATH, "test1.txt");

    // When & Then
    Assertions::assertThrows("Invalid value x, that only support [w, r, a]", [path]() {
        fs::win::File(path, "x");
    });
}

void should_append_win_file() {
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
}

void test_win_file() {
    UnitTestGroup group{"test_win_file"};

    group.addTest("should_write_win_file", should_write_win_file);
    group.addTest("should_fail_to_construct_if_mode_invalid", should_fail_to_construct_if_mode_invalid);
    group.addTest("should_append_win_file", should_append_win_file);

    group.startAll();
}

GROUP_NAME("test_win_file")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(should_write_win_file),
    UNIT_TEST_ITEM(should_fail_to_construct_if_mode_invalid),
    UNIT_TEST_ITEM(should_append_win_file))
} // namespace my::test::test_win_file