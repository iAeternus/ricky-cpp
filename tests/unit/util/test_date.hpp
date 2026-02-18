#ifndef TEST_DATE_HPP
#define TEST_DATE_HPP

namespace my::test::test_date {

void should_construct();
void should_fail_to_construct_by_month_and_day_if_args_invalid();
void should_fail_to_construct_by_day_of_year_if_args_invalid();
void should_parse();
void should_fail_to_parse_if_format_invalid();
void should_fetch_now();
void should_calc_day_of_year();
void should_calc_day_of_week();
void should_plus();
void should_minus();
void should_subtract();
void should_calc_epoch_day();

} // namespace my::test::test_date

#endif // TEST_DATE_HPP