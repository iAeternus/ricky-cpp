#ifndef TEST_UNIT_TEST_HPP
#define TEST_UNIT_TEST_HPP

#include "UnitTest.hpp"

#include <iostream>
#include <exception>
#include <thread>
#include <chrono>

using namespace my::test;

auto testCase1 = []() {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "Test case 1" << std::endl;
};

auto testCase2 = []() {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "Test case 2" << std::endl;
};

auto testCase3 = []() {
    throw std::runtime_error("wa");
};

void should_unit_test() {
    // Given
    UnitTest case1 = UnitTest{"Test case 1", testCase1};
    UnitTest case2 = UnitTest{"Test case 2", testCase2};
    UnitTest case3 = UnitTest{"Test case 3", testCase3};
    case1.start();
    case2.start();
    case3.start();
}

#endif // TEST_UNIT_TEST_HPP