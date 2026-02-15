#include "unit/test_binary_utils.hpp"

#include "ricky_test.hpp"
#include "binary_utils.hpp"

#include "test/test_registry.hpp"

namespace my::test::test_binary_utils {

void should_cal_lowbit() {
    // Given
    u32 x = 0b10110010U;

    // When
    auto res = util::lowbit(x);

    // Then
    Assertions::assertEquals(0b10U, res);
}

void should_cal_lowbit_index() {
    // Given
    u32 x = 0b10110010U;

    // When
    auto res = util::lowbit_idx(x);

    // Then
    Assertions::assertEquals(1U, res);
}

void should_cal_highbit_index() {
    // Given
    u32 x = 0b10110010U;

    // When
    auto res = util::highbit_idx(x);

    // Then
    Assertions::assertEquals(7U, res);
}

void should_cal_highbit() {
    // Given
    u32 x = 0b10110010U;

    // When
    auto res = util::highbit(x);

    // Then
    Assertions::assertEquals(0b10000000U, res);
}

void should_judge_all_one() {
    // Given
    u32 x = 0b11111111U;
    u32 x2 = 0b10110010U;

    // When
    auto res = util::all_one(x);
    auto res2 = util::all_one(x2);

    // Then
    Assertions::assertTrue(res);
    Assertions::assertFalse(res2);
}

void should_judge_only_one() {
    // Given
    u32 x = 0b00010000U;
    u32 x2 = 0b10110010U;

    // When
    auto res = util::only_one(x);
    auto res2 = util::only_one(x2);

    // Then
    Assertions::assertTrue(res);
    Assertions::assertFalse(res2);
}

void should_roundup2() {
    // Given
    isize x = 0;
    isize x2 = 3;
    isize x3 = 4;

    // When
    auto res = util::roundup2(x);
    auto res2 = util::roundup2(x2);
    auto res3 = util::roundup2(x3);

    // Then
    Assertions::assertEquals(0ULL, res);
    Assertions::assertEquals(4ULL, res2);
    Assertions::assertEquals(4ULL, res3);
}

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

GROUP_NAME("test_binary_utils")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(should_cal_lowbit),
    UNIT_TEST_ITEM(should_cal_lowbit_index),
    UNIT_TEST_ITEM(should_cal_highbit_index),
    UNIT_TEST_ITEM(should_cal_highbit),
    UNIT_TEST_ITEM(should_judge_all_one),
    UNIT_TEST_ITEM(should_judge_only_one),
    UNIT_TEST_ITEM(should_roundup2))
} // namespace my::test::test_binary_utils