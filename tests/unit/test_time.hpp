#ifndef TEST_TIME_HPP
#define TEST_TIME_HPP

#include "my_types.hpp"

namespace my::test::test_time {
void should_construct();
void should_fail_to_construct_by_hour_minute_and_second_if_args_invalid();
void should_fail_to_construct_by_second_of_day_if_args_invalid();
void should_parse();
void should_fail_to_parse_if_format_invalid();
void should_fetch_now();
void should_calc_second_of_day();
void should_calc_nanos_of_day();
void should_plus();
void should_minus();
void should_subtract();
void test_time();
} // namespace my::test::test_time

#endif // TEST_TIME_HPP