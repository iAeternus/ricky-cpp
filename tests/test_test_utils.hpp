#ifndef TEST_UNIT_TEST_HPP
#define TEST_UNIT_TEST_HPP

#include "ricky_test.hpp"
#include "my_exception.hpp"

#include <source_location>
#include <thread>
#include <chrono>

namespace my::test::test_test_utils {

auto should_success1 = []() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100LL));
};

auto should_success2 = []() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100LL));
};

auto should_failed = []() {
    throw runtime_exception("wa");
};

auto should_throws = []() {
    Assertions::assertThrows("wa"_cs, should_failed);
    Assertions::assertThrows("This is an IO exception message.", []() {
        throw io_exception("This is an {} message.", "IO exception");
    });
};

auto test_new_test_suite = []() {
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
};

auto test_test_utils() {
    UnitTestGroup group("test_test_utils");

    group.addTest("should_success1", should_success1);
    group.addTest("should_success2", should_success2);
    // group.addTest("should_failed", should_failed);
    group.addTest("should_throws", should_throws);
    group.addTest("test_new_test_suite", test_new_test_suite);

    group.startAll();
}

} // namespace my::test::test_test_utils

#endif // TEST_UNIT_TEST_HPP