#ifndef TEST_UNIT_TEST_HPP
#define TEST_UNIT_TEST_HPP

#include "UnitTest.hpp"
#include "Assertions.hpp"

#include <thread>
#include <chrono>

namespace my::test::test_test_utils {

fn should_success1 = []() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
};

fn should_success2 = []() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
};

fn should_failed = []() {
    throw std::runtime_error("wa");
};

fn should_throws = []() {
    Assertions::assertThrows("wa"_cs, should_failed);
    Assertions::assertThrows("wa", should_failed);
};

fn test_test_utils() {
    UnitTestGroup group("test_test_utils");

    group.addTest("should_success1", should_success1);
    group.addTest("should_success2", should_success2);
    group.addTest("should_failed", should_failed);
    group.addTest("should_throws", should_throws);

    group.startAll();
}

} // namespace my::test::test_test_utils

#endif // TEST_UNIT_TEST_HPP