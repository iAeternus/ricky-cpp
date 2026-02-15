#include "unit/test_test_utils.hpp"

#include "ricky_test.hpp"
#include "my_exception.hpp"

#include <thread>
#include <chrono>

#include "test/test_registry.hpp"

namespace my::test::test_test_utils {

void should_success1() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100LL));
}

void should_success2() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100LL));
}

void should_failed() {
    throw runtime_exception("wa");
}

void should_throws() {
    Assertions::assertThrows("wa"_cs, should_failed);
    Assertions::assertThrows("This is an IO exception message.", []() {
        throw io_exception("This is an {} message.", "IO exception");
    });
}

void test_new_test_suite() {
    i32 a = 1, b = 1, c = 2;
    i32* ptr = nullptr;
    Assertions::assert_equals(a, b);
    Assertions::assert_not_equals(a, c);
    Assertions::assert_null(ptr);
    Assertions::assert_not_null(&a);
    Assertions::assert_true(true);
    Assertions::assert_false(false);
    Assertions::assert_throws<Exception>([]() {
        throw runtime_exception("wa");
    });
    Assertions::assert_throws<Exception>("expect message", []() {
        throw runtime_exception("expect message");
    });
    Assertions::assert_not_throws<Exception>([]() {
        // Something not throws exception
    });
    Assertions::assert_not_throws<Exception>("expect message", []() {
        // Something not throws exception
    });
}

void test_test_utils() {
    UnitTestGroup group("test_test_utils");

    group.addTest("should_success1", should_success1);
    group.addTest("should_success2", should_success2);
    // group.addTest("should_failed", should_failed);
    group.addTest("should_throws", should_throws);
    group.addTest("test_new_test_suite", test_new_test_suite);

    group.startAll();
}

GROUP_NAME("test_test_utils")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(should_success1),
    UNIT_TEST_ITEM(should_success2),
    UNIT_TEST_ITEM(should_throws),
    UNIT_TEST_ITEM(test_new_test_suite))
} // namespace my::test::test_test_utils