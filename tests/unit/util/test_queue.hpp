#ifndef TEST_QUEUE_HPP
#define TEST_QUEUE_HPP

namespace my::test::test_queue {

void it_works();
void should_fail_to_pop_if_queue_is_empty();
void should_fail_to_get_front_if_queue_is_empty();
void should_fail_to_get_tail_if_queue_is_empty();

} // namespace my::test::test_queue

#endif // TEST_QUEUE_HPP