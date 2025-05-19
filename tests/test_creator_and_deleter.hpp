#ifndef TEST_CREATOR_AND_DELETER_HPP
#define TEST_CREATOR_AND_DELETER_HPP

#include "ricky_test.hpp"
#include "Creator.hpp"
#include "Deleter.hpp"

#include <memory>

namespace my::test::test_creator_and_deleter {

/**
 * @brief 辅助类：记录构造和析构次数
 */
class TrackedObject : public Object<TrackedObject> {
public:
    static i32 construct_count;
    static i32 destruct_count;

    TrackedObject(i32 value) :
            value_(value) {
        construct_count++;
    }

    ~TrackedObject() {
        destruct_count++;
    }

    i32 value() const { return value_; }

private:
    i32 value_;
};

i32 TrackedObject::construct_count = 0;
i32 TrackedObject::destruct_count = 0;

/**
 * @brief 异常测试类：构造函数可能抛出异常
 */
class ExceptionProne : public Object<ExceptionProne> {
public:
    explicit ExceptionProne(bool should_throw) {
        if (should_throw) {
            throw std::runtime_error("Construction failed");
        }
    }
};

fn befor_each = []() {
    TrackedObject::construct_count = 0;
    TrackedObject::destruct_count = 0;
};

fn it_works = []() {
    // Given
    util::Creator<TrackedObject> creator;
    util::Deleter<TrackedObject> deleter;

    // When
    TrackedObject* obj = creator(42);

    // Then
    Assertions::assertNotEquals(nullptr, static_cast<void*>(obj));
    Assertions::assertEquals(42, obj->value());
    Assertions::assertEquals(1, TrackedObject::construct_count);

    // When
    deleter(obj);

    // Then
    // Assertions::assertEquals(nullptr, static_cast<void*>(obj));
    Assertions::assertEquals(1, TrackedObject::destruct_count);
};

fn should_delete_array = []() {
    // Given
    constexpr isize ARRAY_SIZE = 5;
    TrackedObject* arr = my_alloc<TrackedObject>(ARRAY_SIZE);
    util::Deleter<TrackedObject[]> array_deleter;

    // When
    for (i32 i = 0; i < ARRAY_SIZE; ++i) {
        my_construct(&arr[i], i);
    }

    // Then
    Assertions::assertEquals(ARRAY_SIZE, TrackedObject::construct_count);

    // When
    array_deleter(arr, ARRAY_SIZE);

    // Then
    Assertions::assertEquals(ARRAY_SIZE, TrackedObject::destruct_count);
};

fn should_exception_safety = []() {
    // Given
    util::Creator<ExceptionProne> creator;

    // When & Then
    Assertions::assertThrows("Construction failed", [&]() {
        creator(true);
    });
};

fn should_integrate_smart_pointer = []() {
    // Given
    util::Creator<TrackedObject> creator;
    util::Deleter<TrackedObject> deleter;

    // When
    TrackedObject* raw_ptr = creator(100);
    std::unique_ptr<TrackedObject, util::Deleter<TrackedObject>> smart_ptr(raw_ptr, deleter);

    // Then
    Assertions::assertEquals(1, TrackedObject::construct_count);
    Assertions::assertEquals(100, smart_ptr->value());

    // When
    smart_ptr.reset();

    // Then
    Assertions::assertEquals(1, TrackedObject::destruct_count);
};

fn should_handle_null_pointer = []() {
    // Given
    util::Deleter<TrackedObject> deleter;
    TrackedObject* ptr = nullptr;

    // When & Then
    deleter(ptr);
};

fn test_creator_and_deleter() {
    UnitTestGroup group{"test_creator_and_deleter"};
    group.before_each(befor_each);

    group.addTest("it_works", it_works);
    group.addTest("should_delete_array", should_delete_array);
    group.addTest("should_exception_safety", should_exception_safety);
    group.addTest("should_integrate_smart_pointer", should_integrate_smart_pointer);
    group.addTest("should_handle_null_pointer", should_handle_null_pointer);

    group.startAll();
}

} // namespace my::test::test_creator_and_deleter

#endif // TEST_CREATOR_AND_DELETER_HPP