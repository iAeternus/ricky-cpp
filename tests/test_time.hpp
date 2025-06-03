#ifndef TEST_TIME_HPP
#define TEST_TIME_HPP

#include "UnitTest.hpp"
#include "Assertions.hpp"
#include "DateTime.hpp"

namespace my::test::test_time {

fn should_construct = []() {
    auto t = util::Time::of(20);
    auto t2 = util::Time::of(12, 34, 56, 789);

    Assertions::assertEquals("20:00:00.000000000"_cs, t.__str__());
    Assertions::assertEquals("12:34:56.000000789"_cs, t2.__str__());
};

fn should_parse = []() {
    // Given
    CString str = "20:32:06";

    // When
    auto t = util::Time::parse(str);

    // Then
    Assertions::assertEquals(20, t.hour());
    Assertions::assertEquals(32, t.minute());
    Assertions::assertEquals(6, t.second());
};

fn should_fetch_now = []() {
    auto t = util::Time::now();

    io::println(t);
};

fn should_calc_second_of_day = []() {
    // Given
    auto t = util::Time::of(0, 0, 0);
    auto t2 = util::Time::of(20, 20, 20);
    auto t3 = util::Time::of(23, 59, 59);

    // When
    auto res = util::Time::ofSecondOfDay(t.toSecondOfDay());
    auto res2 = util::Time::ofSecondOfDay(t2.toSecondOfDay());
    auto res3 = util::Time::ofSecondOfDay(t3.toSecondOfDay());

    // Then
    Assertions::assertEquals(t, res);
    Assertions::assertEquals(t2, res2);
    Assertions::assertEquals(t3, res3);
};

fn should_calc_nanos_of_day = []() {
    // Given
    auto t = util::Time::MIN;
    auto t2 = util::Time::of(20, 20, 20, 20);
    auto t3 = util::Time::MAX;

    // When
    auto res = util::Time::ofNanoOfDay(t.toNanoOfDay());
    auto res2 = util::Time::ofNanoOfDay(t2.toNanoOfDay());
    auto res3 = util::Time::ofNanoOfDay(t3.toNanoOfDay());

    // Then
    Assertions::assertEquals(t, res);
    Assertions::assertEquals(t2, res2);
    Assertions::assertEquals(t3, res3);
};

fn should_plus = []() {
    // Given
    auto t = util::Time::of(6);

    // When
    auto t2 = t.plusHours(1);     // 7:00
    auto t3 = t2.plusMinutes(61); // 8:01
    auto t4 = t3.plusSeconds(61); // 8:02:01

    // Then
    Assertions::assertEquals("08:02:01.000000000"_cs, t4.__str__());
};

fn should_minus = []() {
    // Given
    auto t = util::Time::of(18);

    // When
    auto t2 = t.minusHours(1);     // 17:00
    auto t3 = t2.minusMinutes(61); // 15:59
    auto t4 = t3.minusSeconds(61); // 15:57:59

    // Then
    Assertions::assertEquals("15:57:59.000000000"_cs, t4.__str__());
};

fn should_subtract = []() {
    // Given
    auto t = util::Time::of(20, 20, 20, 20);
    auto t2 = util::Time::of(18);
    auto t3 = util::Time::of(22);

    // When
    auto res = t - t2;
    auto res2 = t - t3;

    // Then
    Assertions::assertEquals("PT2H20M20.000000020S"_cs, res.__str__());
    Assertions::assertEquals("PT-1H-39M-40.000000020S"_cs, res2.__str__());
};

fn test_time() {
    UnitTestGroup group{"test_time"};

    group.addTest("should_construct", should_construct);
    group.addTest("should_parse", should_parse);
    group.addTest("should_fetch_now", should_fetch_now);
    group.addTest("should_calc_second_of_day", should_calc_second_of_day);
    group.addTest("should_calc_nanos_of_day", should_calc_nanos_of_day);
    group.addTest("should_plus", should_plus);
    group.addTest("should_minus", should_minus);
    group.addTest("should_subtract", should_subtract);

    group.startAll();
}

} // namespace my::test::test_time

#endif // TEST_TIME_HPP