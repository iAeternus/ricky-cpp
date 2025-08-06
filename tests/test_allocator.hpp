#ifndef TEST_ALLOCATOR_HPP
#define TEST_ALLOCATOR_HPP

#include "ricky_test.hpp"
#include "Allocator.hpp"
#include "ricky.hpp"

namespace my::test::test_allocator {

/**
 * @brief 测试用对象
 */
class TestObject {
public:
    TestObject(int v) :
            value(v) { ++count; }
    ~TestObject() { --count; }

    i32 value;
    static i32 count;
};
inline i32 TestObject::count = 0;

/**
 * @brief 异常测试类
 */
class ThrowingConstructor {
public:
    ThrowingConstructor() { throw std::runtime_error("Test Exception"); }
};

fn should_allocate_and_deallocate = []() {
    // Given
    Allocator<i32> alloc;
    constexpr usize n = 10;

    // When
    i32* ptr = alloc.allocate(n);

    // Then
    Assertions::assertTrue(ptr);

    // Finally
    alloc.deallocate(ptr, n); // 不应崩溃
};

fn should_zero_allocate = []() {
    // Given
    Allocator<i32> alloc;

    // When
    i32* ptr = alloc.allocate(0);

    // When
    Assertions::assertFalse(ptr);
};

// TODO 需要改进这个测试点
fn should_fail_to_allocate_if_exceeds_max_size = []() {
    // Given
    Allocator<i32> alloc;

    // When & Then
    Assertions::assertThrows("", [&]() {
        alloc.allocate(usize(-1)); // 尝试分配超过最大大小
    });
};

fn should_allocate_object = []() {
    // Given
    Allocator<TestObject> alloc;
    TestObject::count = 0;

    // When
    TestObject* obj = alloc.allocate(1);
    alloc.construct(obj, 42);

    // Then
    Assertions::assertEquals(42, obj->value);
    Assertions::assertEquals(1, TestObject::count);

    // When
    alloc.destroy(obj);

    // Then
    Assertions::assertEquals(0, TestObject::count);

    // Finally
    alloc.deallocate(obj, 1); // 不应崩溃
};

fn should_multi_object_destruction = []() {
    // Given
    Allocator<TestObject> alloc;
    constexpr usize n = 5;
    TestObject::count = 0;

    // When
    TestObject* objs = alloc.allocate(n);
    for (int i = 0; i < n; ++i) {
        alloc.construct(objs + i, i);
    }

    // Then
    Assertions::assertEquals(n, TestObject::count);

    // When
    alloc.destroy(objs, n);

    // Then
    Assertions::assertEquals(0, TestObject::count);

    // Finally
    alloc.deallocate(objs, n); // 不应崩溃
};

fn should_safe_create_object = []() {
    // Given
    Allocator<TestObject> alloc;
    TestObject::count = 0;

    // When
    auto* obj = alloc.create(42);

    // Then
    Assertions::assertTrue(obj);
    Assertions::assertEquals(42, obj->value);
    Assertions::assertEquals(1, TestObject::count);

    // Finally
    alloc.destroy(obj);       // 不应崩溃
    alloc.deallocate(obj, 1); // 不应崩溃
};

fn should_container_integration = []() {
    // Given
    Allocator<i32> alloc;
    std::vector<i32, Allocator<i32>> vec(alloc);

    // When
    for (i32 i = 0; i < 100; ++i) {
        vec.push_back(i);
    }

    // Then
    for (i32 i = 0; i < 100; ++i) {
        Assertions::assertEquals(i, vec[i]);
    }
};

fn should_cmp = []() {
    // Given
    Allocator<i32> alloc1;
    Allocator<f64> alloc2;

    // When & Then
    Assertions::assertTrue(alloc1 == alloc2);
    Assertions::assertFalse(alloc1 != alloc2);
};

fn test_allocator() {
    UnitTestGroup group("test_allocator");

    group.addTest("should_allocate_and_deallocate", should_allocate_and_deallocate);
    group.addTest("should_zero_allocate", should_zero_allocate);
    // group.addTest("should_fail_to_allocate_if_exceeds_max_size", should_fail_to_allocate_if_exceeds_max_size);
    group.addTest("should_allocate_object", should_allocate_object);
    group.addTest("should_multi_object_destruction", should_multi_object_destruction);
    group.addTest("should_safe_create_object", should_safe_create_object);
    group.addTest("should_container_integration", should_container_integration);
    group.addTest("should_cmp", should_cmp);

    group.startAll();
}

} // namespace my::test::test_allocator

#endif // TEST_ALLOCATOR_HPP