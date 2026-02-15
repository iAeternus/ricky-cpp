#ifndef TEST_DATE_TIME_HPP
#define TEST_DATE_TIME_HPP

#include "my_types.hpp"

namespace my::test::test_date_time {
void should_construct();
void should_parse();
void should_fail_to_parse_if_format_invalid();
void should_fetch_now();
void should_construct_by_epoch_second();
void should_plus();
void should_minus();
void should_subtract();
void test_date_time();
} // namespace my::test::test_date_time

#endif // TEST_DATE_TIME_HPP