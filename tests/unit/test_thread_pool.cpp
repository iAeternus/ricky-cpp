#include "test_thread_pool.hpp"
#include "my_exception.hpp"
#include "thread_pool.hpp"
#include "hash_map.hpp"
#include "ricky_test.hpp"

namespace my::test::test_thread_pool {

inline i32 add(i32 a, i32 b) {
    return a + b;
}

void throw_exception() {
    throw runtime_exception("wa");
}

void should_push() {
    // Given
    async::ThreadPool tp{4};

    // When
    auto future = tp.push(add, 2, 3);

    // Then
    Assertions::assertEquals(5, future.get());
}

void should_push_tasks() {
    // Given
    async::ThreadPool tp{4};

    // When
    auto future = tp.push(add, 2, 3);
    auto future2 = tp.push(add, 4, 5);

    // Then
    Assertions::assertEquals(5, future.get());
    Assertions::assertEquals(9, future2.get());
}

void should_push_tasks_with_exception() {
    // Given
    async::ThreadPool tp{4};

    // When
    auto future = tp.push(throw_exception);
    auto future2 = tp.push([]() {
        throw runtime_exception("wa2");
    });

    // Then
    Assertions::assertThrows("wa", [&]() {
        future.get();
    });

    Assertions::assertThrows("wa2", [&]() {
        future2.get();
    });
}

void should_wait() {
    // Given
    usize n = 100;
    async::ThreadPool tp{4};
    util::HashMap<i32, i32> futures;

    // When
    for (usize i = 0; i < n; ++i) {
        futures.insert(i, tp.push(add, i, i).get());
    }
    tp.wait();

    // Then
    for (usize i = 0; i < n; ++i) {
        Assertions::assertEquals(i * 2, static_cast<usize>(futures[i]));
    }
}

GROUP_NAME("test_thread_pool")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(should_push),
    UNIT_TEST_ITEM(should_push_tasks),
    UNIT_TEST_ITEM(should_push_tasks_with_exception),
    UNIT_TEST_ITEM(should_wait))

} // namespace my::test::test_thread_pool
