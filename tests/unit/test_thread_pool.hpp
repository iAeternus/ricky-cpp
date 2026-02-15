#ifndef TEST_THREAD_POOL_HPP
#define TEST_THREAD_POOL_HPP

namespace my::test::test_thread_pool {

void throw_exception();
void should_push();
void should_push_tasks();
void should_push_tasks_with_exception();
void should_wait();

void speed_of_thread_pool();
void speed_of_sync();
void test_thread_pool_speed();

} // namespace my::test::test_thread_pool

#endif // TEST_THREAD_POOL_HPP
