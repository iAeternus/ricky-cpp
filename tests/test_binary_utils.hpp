#ifndef TEST_BINARY_UTILS_HPP
#define TEST_BINARY_UTILS_HPP

#include "ricky_test.hpp"
#include "binary_utils.hpp"

namespace my::test::test_binary_utils {

auto should_cal_lowbit = []() {
    // Given
    u32 x = 0b10110010U;

    // When
    auto res = util::lowbit(x);

    // Then
    Assertions::assertEquals(0b10U, res);
};

auto should_cal_lowbit_index = []() {
    // Given
    u32 x = 0b10110010U;

    // When
    auto res = util::lowbit_index(x);

    // Then
    Assertions::assertEquals(1U, res);
};

auto should_cal_highbit_index = []() {
    // Given
    u32 x = 0b10110010U;

    // When
    auto res = util::highbit_index(x);

    // Then
    Assertions::assertEquals(7U, res);
};

auto should_cal_highbit = []() {
    // Given
    u32 x = 0b10110010U;

    // When
    auto res = util::highbit(x);

    // Then
    Assertions::assertEquals(0b10000000U, res);
};

auto should_judge_all_one = []() {
    // Given
    u32 x = 0b11111111U;
    u32 x2 = 0b10110010U;

    // When
    auto res = util::all_one(x);
    auto res2 = util::all_one(x2);

    // Then
    Assertions::assertTrue(res);
    Assertions::assertFalse(res2);
};

auto should_judge_only_one = []() {
    // Given
    u32 x = 0b00010000U;
    u32 x2 = 0b10110010U;

    // When
    auto res = util::only_one(x);
    auto res2 = util::only_one(x2);

    // Then
    Assertions::assertTrue(res);
    Assertions::assertFalse(res2);
};

auto should_roundup2 = []() {
    // Given
    isize x = 0;
    isize x2 = 3;
    isize x3 = 4;

    // When
    auto res = util::roundup2(x);
    auto res2 = util::roundup2(x2);
    auto res3 = util::roundup2(x3);

    // Then
    Assertions::assertEquals(0, res);
    Assertions::assertEquals(4, res2);
    Assertions::assertEquals(4, res3);
};

void test_binary_utils() {
    UnitTestGroup group{"test_binary_utils"};

    group.addTest("should_cal_lowbit", should_cal_lowbit);
    group.addTest("should_cal_lowbit_index", should_cal_lowbit_index);
    group.addTest("should_cal_highbit_index", should_cal_highbit_index);
    group.addTest("should_cal_highbit", should_cal_highbit);
    group.addTest("should_judge_all_one", should_judge_all_one);
    group.addTest("should_judge_only_one", should_judge_only_one);
    group.addTest("should_roundup2", should_roundup2);

    group.startAll();
}

} // namespace my::test::test_binary_utils

#endif // TEST_BINARY_UTILS_HPP