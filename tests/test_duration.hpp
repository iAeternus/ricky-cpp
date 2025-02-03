#ifndef TEST_DURATION_HPP
#define TEST_DURATION_HPP

#include "ricky_test.hpp"
#include "Duration.hpp"

namespace my::test::test_duration {

auto should_construct = []() {
    auto d = util::Duration::ofDays(4);
    auto d2 = util::Duration::ofHours(4);
    auto d3 = util::Duration::ofMinutes(4);
    auto d4 = util::Duration::ofSeconds(4);
    auto d5 = util::Duration::ofMillis(4);
    auto d6 = util::Duration::ofNanos(4);
    auto d7 = util::Duration::ZERO;

    Assertions::assertEquals(4LL, d.toDays());
    Assertions::assertEquals(96LL, d.toHours());
    Assertions::assertEquals(5760LL, d.toMinutes());
    Assertions::assertEquals(345600LL, d.getSeconds());
    Assertions::assertEquals(0, d.getNanos());

    Assertions::assertEquals("PT4D"_cs, d.__str__());
    Assertions::assertEquals("PT4H"_cs, d2.__str__());
    Assertions::assertEquals("PT4M"_cs, d3.__str__());
    Assertions::assertEquals("PT4S"_cs, d4.__str__());
    Assertions::assertEquals("PT0.004000000S"_cs, d5.__str__());
    Assertions::assertEquals("PT0.000000004S"_cs, d6.__str__());
    Assertions::assertEquals("PT0.0S"_cs, d7.__str__());
};

auto should_add = []() {
    // Given
    auto d = util::Duration::ofHours(25);
    auto d2 = util::Duration::ofDays(1);

    // When
    auto res = d + d2;

    // Then
    Assertions::assertEquals(49LL, res.toHours());
};

auto should_substract = []() {
    // Given
    auto d = util::Duration::ofHours(25);
    auto d2 = util::Duration::ofDays(1);

    // When
    auto res = d - d2;

    // Then
    Assertions::assertEquals(1LL, res.toHours());
};

auto should_multiply = []() {
    // Given
    auto d = util::Duration::ofMinutes(15);
    i64 scalar = 2;

    // When
    auto res = d * scalar;

    // Then
    Assertions::assertEquals(30LL, res.toMinutes());
};

void test_duration() {
    UnitTestGroup group{"test_duration"};

    group.addTest("should_construct", should_construct);
    group.addTest("should_add", should_add);
    group.addTest("should_substract", should_substract);
    group.addTest("should_multiply", should_multiply);

    group.startAll();
}

} // namespace my::test::test_duration

#endif // TEST_DURATION_HPP