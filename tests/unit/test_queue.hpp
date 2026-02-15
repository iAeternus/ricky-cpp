#ifndef TEST_QUEUE_HPP
#define TEST_QUEUE_HPP

#include "my_types.hpp"

namespace my::test::test_queue {
void it_works();
void should_fail_to_pop_if_queue_is_empty();
void should_fail_to_get_front_if_queue_is_empty();
void should_fail_to_get_tail_if_queue_is_empty();
void test_queue();
void speed_of_util_queue_push_and_pop();
void speed_of_std_queue_push_and_pop();
void test_queue_speed();
} // namespace my::test::test_queue

#endif // TEST_QUEUE_HPP