#ifndef TEST_DATE_HPP
#define TEST_DATE_HPP

#include "ricky_test.hpp"
#include "DateTime.hpp"

namespace my::test::test_date {

auto should_construct = []() {
    auto d = util::Date::of(1970);
    auto d2 = util::Date::of(2025, 2, 3);

    Assertions::assertEquals(1970, d.year());
    Assertions::assertEquals(1, d.month());
    Assertions::assertEquals(1, d.day());
    Assertions::assertEquals("1970-01-01"_cs, d.__str__());
    Assertions::assertEquals("2025-02-03"_cs, d2.__str__());
};

auto should_parse = []() {
    // Given
    CString str = "2025-02-04";

    // When
    auto d = util::Date::parse(str);

    // Then
    Assertions::assertEquals(2025, d.year());
    Assertions::assertEquals(2, d.month());
    Assertions::assertEquals(4, d.day());
};

auto should_fetch_now = []() {
    auto d = util::Date::now();

    io::println(d);
};

auto should_calc_day_of_year = []() {
    // Given
    auto d = util::Date::of(2025, 2, 4);
    auto d2 = util::Date::of(1970);
    auto d3 = util::Date::of(2024, 2, 29);
    auto d4 = util::Date::of(2024, 12, 31);

    // When
    auto res = util::Date::ofYearDay(d.year(), d.dayOfYear());
    auto res2 = util::Date::ofYearDay(d2.year(), d2.dayOfYear());
    auto res3 = util::Date::ofYearDay(d3.year(), d3.dayOfYear());
    auto res4 = util::Date::ofYearDay(d4.year(), d4.dayOfYear());

    // Then
    Assertions::assertEquals(d, res);
    Assertions::assertEquals(d2, res2);
    Assertions::assertEquals(d3, res3);
    Assertions::assertEquals(d4, res4);
};

auto should_calc_day_of_week = []() {
    // Given
    auto d = util::Date::of(2025, 2, 4);   // 周二
    auto d2 = util::Date::of(2025, 1, 27); // 周一
    auto d3 = util::Date::of(2024, 2, 29); // 周四

    // When
    auto res = d.dayOfWeek();
    auto res2 = d2.dayOfWeek();
    auto res3 = d3.dayOfWeek();

    // Then
    Assertions::assertEquals(2, res);
    Assertions::assertEquals(1, res2);
    Assertions::assertEquals(4, res3);
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
    auto d3 = util::Date::of(2026, 3, 5);

    // When
    auto res = d - d2;
    auto res2 = d - d3;

    // Then
    Assertions::assertEquals("PT398D"_cs, res.__str__());
    Assertions::assertEquals("PT-394D"_cs, res2.__str__());
};

auto should_calc_epoch_day = []() {
    // Given
    auto d = util::Date::of(2025, 2, 4);
    auto d2 = util::Date::of(1970);
    auto d3 = util::Date::of(2024, 2, 29);
    auto d4 = util::Date::of(2024, 12, 31);

    // When
    auto res = util::Date::fromEpochDay(d.toEpochDay());
    auto res2 = util::Date::fromEpochDay(d2.toEpochDay());
    auto res3 = util::Date::fromEpochDay(d3.toEpochDay());
    auto res4 = util::Date::fromEpochDay(d4.toEpochDay());

    // Then
    Assertions::assertEquals(d, res);
    Assertions::assertEquals(d2, res2);
    Assertions::assertEquals(d3, res3);
    Assertions::assertEquals(d4, res4);
};

void test_date() {
    UnitTestGroup group{"test_date"};

    group.addTest("should_construct", should_construct);
    group.addTest("should_parse", should_parse);
    group.addTest("should_fetch_now", should_fetch_now);
    group.addTest("should_calc_day_of_year", should_calc_day_of_year);
    group.addTest("should_calc_day_of_week", should_calc_day_of_week);
    group.addTest("should_plus", should_plus);
    group.addTest("should_minus", should_minus);
    group.addTest("should_subtract", should_subtract);
    group.addTest("should_calc_epoch_day", should_calc_epoch_day);

    group.startAll();
}

} // namespace my::test::test_date

#endif // TEST_DATE_TIME_HPP