#ifndef TEST_DATE_TIME_HPP
#define TEST_DATE_TIME_HPP

#include "UnitTest.hpp"
#include "Assertions.hpp"
#include "DateTime.hpp"

namespace my::test::test_date_time {

fn should_construct = []() {
    auto dt = util::DateTime::of(2025, 2, 5, 20, 20, 20, 20);
    auto dt2 = util::DateTime::of(util::Date::of(2025, 2, 5), util::Time::of(23, 59, 59));

    Assertions::assertEquals(2025, dt.year());
    Assertions::assertEquals(2, dt.month());
    Assertions::assertEquals(5, dt.day());
    Assertions::assertEquals(20, dt.hour());
    Assertions::assertEquals(20, dt.minute());
    Assertions::assertEquals(20, dt.second());
    Assertions::assertEquals(20, dt.nano());
    Assertions::assertEquals("2025-02-05T20:20:20.000000020Z"_cs, dt.__str__());
    Assertions::assertEquals("2025-02-05T23:59:59.000000000Z"_cs, dt2.__str__());
};

fn should_parse = []() {
    // Given
    CString str = "2025-02-05 20:20:20";

    // When
    auto dt = util::DateTime::parse(str);

    // Then
    Assertions::assertEquals("2025-02-05T20:20:20.000000000Z"_cs, dt.__str__());
};

fn should_fetch_now = []() {
    auto dt = util::DateTime::now();

    io::println(dt);
};

fn should_construct_by_epoch_second = []() {
    // Given
    i64 epochSecond = 1738752308;

    // When
    auto dt = util::DateTime::ofEpochSecond(epochSecond);

    // Then
    Assertions::assertEquals("2025-02-05T10:45:08.000000000Z"_cs, dt.__str__());
};

fn should_plus = []() {
    // Given
    auto dt = util::DateTime::of(2025, 2, 5, 20);

    // When
    auto dt2 = dt.plusYears(1);    // 2026-2-5 20:00
    auto dt3 = dt2.plusMonths(1);  // 2026-3-5 20:00
    auto dt4 = dt3.plusWeeks(1);   // 2026-3-12 20:00
    auto dt5 = dt4.plusDays(1);    // 2026-3-13 20:00
    auto dt6 = dt5.plusHours(1);   // 2026-3-13 21:00
    auto dt7 = dt6.plusMinutes(1); // 2026-3-13 21:01
    auto dt8 = dt7.plusSeconds(1); // 2026-3-13 21:01:01
    auto dt9 = dt8.plusNanos(1);   // 2026-3-13 21:01:01.000000001

    // Then
    Assertions::assertEquals("2026-03-13T21:01:01.000000001Z"_cs, dt9.__str__());
};

fn should_minus = []() {
    // Given
    auto dt = util::DateTime::of(2025, 2, 5, 20);

    // When
    auto dt2 = dt.minusYears(1);    // 2024-2-5 20:00
    auto dt3 = dt2.minusMonths(1);  // 2024-1-5 20:00
    auto dt4 = dt3.minusWeeks(1);   // 2023-12-29 20:00
    auto dt5 = dt4.minusDays(1);    // 2023-12-28 20:00
    auto dt6 = dt5.minusHours(1);   // 2023-12-28 19:00
    auto dt7 = dt6.minusMinutes(1); // 2023-12-28 18:59
    auto dt8 = dt7.minusSeconds(1); // 2023-12-28 18:58:59
    auto dt9 = dt8.minusNanos(1);   // 2023-12-28 18:58:58.999999999

    // Then
    Assertions::assertEquals("2023-12-28T18:58:58.999999999Z"_cs, dt9.__str__());
};

fn should_subtract = []() {
    // Given
    auto dt = util::DateTime::of(2025, 2, 5, 20);
    auto dt2 = util::DateTime::of(2025, 2, 5, 10);
    auto dt3 = util::DateTime::of(2025, 2, 6, 6);

    // When
    auto res = dt - dt2;
    auto res2 = dt - dt3;

    // Then
    Assertions::assertEquals("PT10H"_cs, res.__str__());
    Assertions::assertEquals("PT-10H"_cs, res2.__str__());
};

fn test_date_time() {
    UnitTestGroup group{"test_date_time"};

    group.addTest("should_construct", should_construct);
    group.addTest("should_parse", should_parse);
    group.addTest("should_fetch_now", should_fetch_now);
    group.addTest("should_construct_by_epoch_second", should_construct_by_epoch_second);
    group.addTest("should_plus", should_plus);
    group.addTest("should_minus", should_minus);
    group.addTest("should_subtract", should_subtract);

    group.startAll();
}

} // namespace my::test::test_date_time

#endif // TEST_DATE_TIME_HPP