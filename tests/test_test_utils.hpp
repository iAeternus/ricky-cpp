#ifndef TEST_UNIT_TEST_HPP
#define TEST_UNIT_TEST_HPP

#include "UnitTest.hpp"

#include <iostream>
#include <exception>
#include <thread>
#include <chrono>

using namespace my;

auto testCase1 = []() {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
};

auto testCase2 = []() {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
};

auto testCase3 = []() {
    throw std::runtime_error("wa");
};

void should_group_unit_test() {
    test::UnitTestGroup group("should_group_unit_test");

    group.addTest("Test case 1", testCase1);
    group.addTest("Test case 2", testCase2);
    group.addTest("Test case 3", testCase3);

    group.startAll();
}

#endif // TEST_UNIT_TEST_HPP