#ifndef TEST_THREAD_POOL_HPP
#define TEST_THREAD_POOL_HPP

#include "ricky_test.hpp"
#include "ThreadPool.hpp"
#include "Dict.hpp"

namespace my::test::test_thread_pool {

fn add = [](i32 a, i32 b) {
    return a + b;
};

fn throw_exception = []() {
    throw std::runtime_error("wa");
};

fn should_push = []() {
    // Given
    async::ThreadPool tp{4};

    // When
    auto future = tp.push(add, 2, 3);

    // Then
    Assertions::assertEquals(5, future.get());
};

fn should_push_tasks = []() {
    // Given
    async::ThreadPool tp{4};

    // When
    auto future = tp.push(add, 2, 3);
    auto future2 = tp.push(add, 4, 5);

    // Then
    Assertions::assertEquals(5, future.get());
    Assertions::assertEquals(9, future2.get());
};

fn should_push_tasks_with_exception = []() {
    // Given
    async::ThreadPool tp{4};

    // When
    auto future = tp.push(throw_exception);
    auto future2 = tp.push([]() {
        throw std::runtime_error("wa2");
    });

    // Then
    Assertions::assertThrows("wa", [&]() {
        future.get();
    });

    Assertions::assertThrows("wa2", [&]() {
        future2.get();
    });
};

fn should_wait = []() {
    // Given
    i32 n = 100;
    async::ThreadPool tp{4};
    util::Dict<i32, i32> futures;

    // When
    for (i32 i = 0; i < n; ++i) {
        futures.insert(i, tp.push(add, i, i).get());
    }
    tp.wait();

    // Then
    for (i32 i = 0; i < n; ++i) {
        Assertions::assertEquals(i * 2, futures[i]);
    }
};

fn test_thread_pool() {
    UnitTestGroup group{"test_thread_pool"};

    group.addTest("should_push", should_push);
    group.addTest("should_push_tasks", should_push_tasks);
    group.addTest("should_push_tasks_with_exception", should_push_tasks_with_exception);
    group.addTest("should_wait", should_wait);

    group.startAll();
}

i32 n; // count of tasks
fn task = []() {
    std::this_thread::sleep_for(std::chrono::milliseconds(3));
};

fn speed_of_thread_pool = []() {
    async::ThreadPool tp{100};
    for (i32 i = 0; i < n; ++i) {
        tp.push(task);
    }
    tp.wait();
};

fn speed_of_sync = []() {
    for (i32 i = 0; i < n; ++i) {
        task();
    }
};

fn test_thread_pool_speed() {
    UnitTestGroup group{"test_thread_pool_speed"};
    group.setup([]() {
        n = 100;
    });

    group.addTest("speed_of_thread_pool", speed_of_thread_pool);
    group.addTest("speed_of_sync", speed_of_sync);

    group.startAll();
}

} // namespace my::test::test_thread_pool

#endif // TEST_THREAD_POOL_HPP