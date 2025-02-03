#ifndef TEST_DATE_TIME_HPP
#define TEST_DATE_TIME_HPP

#include "ricky_test.hpp"
#include "DateTime.hpp"

namespace my::test::test_date_time {

auto should_construct_date = []() {
    auto d = util::Date::of(1970);
    auto d2 = util::Date::of(2025, 2, 3);

    Assertions::assertEquals(1970, d.year());
    Assertions::assertEquals(1, d.month());
    Assertions::assertEquals(1, d.day());
    Assertions::assertEquals("1970-01-01"_cs, d.__str__());
    Assertions::assertEquals("2025-02-03"_cs, d2.__str__());
};

auto should_plus = []() {
    // Given
    auto d = util::Date::of(1970);

    // When
    auto d2 = d.plusDays(366);   // 1971-01-02
    auto d3 = d2.plusMonths(13); // 1972-02-02
    auto d4 = d3.plusYears(2);   // 1974-02-02

    // Then
    Assertions::assertEquals("1974-02-02"_cs, d4.__str__());
};

auto should_minus = []() {
    // Given
    auto d = util::Date::of(1970);

    // When
    auto d2 = d.minusDays(366);   // 1968-12-31
    auto d3 = d2.minusMonths(13); // 1967-11-30
    auto d4 = d3.minusYears(2);   // 1965-11-30

    // Then
    Assertions::assertEquals("1965-11-30"_cs, d4.__str__());
};

auto should_subtract = []() {
    // Given
    auto d = util::Date::of(2025, 2, 4);
    auto d2 = util::Date::of(2024, 1, 3);

    // When
    auto res = d - d2;

    // Then
    Assertions::assertEquals("PT398D"_cs, res.__str__());
};

void test_date_time() {
    UnitTestGroup group{"test_date_time"};

    group.addTest("should_construct_date", should_construct_date);
    group.addTest("should_plus", should_plus);
    group.addTest("should_minus", should_minus);
    group.addTest("should_subtract", should_subtract);

    group.startAll();
}

} // namespace my::test::test_date_time

#endif // TEST_DATE_TIME_HPP