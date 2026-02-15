#include "unit/test_allocator.hpp"

#include <vector>
#include <list>

#include "ricky_test.hpp"
#include "binary_utils.hpp"
#include "alloc.hpp"
#include "str.hpp"

#include "test/test_registry.hpp"

namespace my::test::test_allocator {

template <typename T>
using Alloc = mem::Allocator<T>;

/**
 * @brief 带资源的测试对象
 */
class ResourceObject {
public:
    ResourceObject(i32 v, const util::String& name = "") :
            value(v), name(name) {
        ++count;
    }

    ~ResourceObject() {
        --count;
    }

    ResourceObject(const ResourceObject& other) :
            value(other.value), name(other.name) {
        ++count;
    }

    ResourceObject(ResourceObject&& other) noexcept :
            value(other.value), name(std::move(other.name)) {
        ++count;
        other.value = 0;
    }

    i32 value;
    util::String name;
    static i32 count;
};

inline i32 ResourceObject::count = 0;

/**
 * @brief 大对齐类型（用于测试对齐分配）
 */
struct alignas(64) AlignedType {
    f64 data[8]; // 64字节
    i32 id;
};

/**
 * @brief 非平凡析构的类型
 */
class NonTrivial {
public:
    explicit NonTrivial(i32 v) :
            value(new i32(v)) {}
    ~NonTrivial() { delete value; }

    NonTrivial(const NonTrivial&) = delete;
    NonTrivial& operator=(const NonTrivial&) = delete;

    i32* value;
};

/**
 * @brief 构造时抛异常的类型
 */
struct ThrowingConstructor {
    static inline int constructed = 0;
    static inline int alive = 0;
    static inline int call_index = 0;

    bool ok;

    explicit ThrowingConstructor(bool ok_) :
            ok(ok_) {
        if (!ok) {
            throw std::runtime_error("boom");
        }
        ++constructed;
        ++alive;
    }

    ThrowingConstructor() {
        if (call_index++ == 2) {
            throw std::runtime_error("boom");
        }
        ++constructed;
        ++alive;
    }

    ~ThrowingConstructor() {
        --alive;
    }

    static void reset() {
        constructed = 0;
        alive = 0;
        call_index = 0;
    }
};

void test_basic_allocation() {
    Alloc<i32> alloc;

    // 正常分配
    i32* ptr = alloc.allocate(10);
    Assertions::assert_not_null(ptr);
    alloc.deallocate(ptr, 10);

    // 零分配
    ptr = alloc.allocate(0);
    Assertions::assert_null(ptr);

    // 最大分配测试
    std::size_t max = alloc.max_size();
    Assertions::assert_true(max > 0);

    // 边界测试
    ptr = alloc.allocate(1);
    Assertions::assert_not_null(ptr);
    alloc.deallocate(ptr, 1);
}

void test_object_construction() {
    Alloc<ResourceObject> alloc;
    ResourceObject::count = 0;

    // 单个对象构造
    ResourceObject* obj = alloc.allocate(1);
    alloc.construct(obj, 42, "test");

    Assertions::assert_equals(42, obj->value);
    Assertions::assert_equals("test"_s, obj->name);
    Assertions::assert_equals(1, ResourceObject::count);

    alloc.destroy(obj);
    alloc.deallocate(obj, 1);

    Assertions::assert_equals(0, ResourceObject::count);
}

void test_batch_operations() {
    Alloc<ResourceObject> alloc;
    ResourceObject::count = 0;
    constexpr std::size_t N = 10;

    ResourceObject* array = alloc.allocate(N);

    // 批量构造
    for (std::size_t i = 0; i < N; ++i) {
        alloc.construct(array + i, static_cast<i32>(i), util::String("obj_") + util::String::from_i32(i));
    }

    Assertions::assert_equals(N, static_cast<usize>(ResourceObject::count));

    // 验证构造正确性
    for (std::size_t i = 0; i < N; ++i) {
        Assertions::assert_equals(static_cast<i32>(i), array[i].value);
        Assertions::assert_equals(util::String("obj_") + util::String::from_i32(i), array[i].name);
    }

    // 批量销毁
    alloc.destroy_n(array, N);
    Assertions::assert_equals(0, ResourceObject::count);

    alloc.deallocate(array, N);
}

void test_safe_creation() {
    Alloc<ResourceObject> alloc;
    ResourceObject::count = 0;

    // 成功创建
    ResourceObject* obj = alloc.create(100, "safe_object");
    Assertions::assert_not_null(obj);
    Assertions::assert_equals(100, obj->value);
    Assertions::assert_equals(1, ResourceObject::count);

    // 安全销毁
    alloc.destroy(obj);
    alloc.deallocate(obj, 1);
    Assertions::assert_equals(0, ResourceObject::count);

    // 测试异常安全 - 这里需要测试构造失败的情况
    // 注意：create 在异常时会返回 nullptr
    ThrowingConstructor::constructed = 0;

    Alloc<ThrowingConstructor> throwing_alloc;
    ThrowingConstructor* bad_obj = throwing_alloc.create(false);
    Assertions::assert_null(bad_obj); // 构造失败应返回 nullptr
}

void test_aligned_allocation() {
    Alloc<AlignedType> alloc;

    // 测试不同对齐要求
    //    AlignedType* ptr16 = alloc.allocate_aligned<16>(5);
    //    Assertions::assert_not_null(ptr16);
    //    Assertions::assert_true(reinterpret_cast<uintptr_t>(ptr16) % 16 == 0);
    //    alloc.deallocate(ptr16, 5);

    AlignedType* ptr64 = alloc.allocate_aligned<64>(3);
    Assertions::assert_not_null(ptr64);
    Assertions::assert_true(reinterpret_cast<uintptr_t>(ptr64) % 64 == 0);
    alloc.deallocate(ptr64, 3);

    // 测试默认对齐分配
    AlignedType* default_ptr = alloc.allocate(2);
    Assertions::assert_not_null(default_ptr);
    // 默认应满足 alignof(AlignedType) 对齐
    Assertions::assert_true(reinterpret_cast<uintptr_t>(default_ptr) % alignof(AlignedType) == 0);
    alloc.deallocate(default_ptr, 2);
}

void test_over_allocation() {
    Alloc<i32> alloc;

    auto result = alloc.allocate_at_least(7);
    Assertions::assert_not_null(result.ptr);

    // 结果应该是2的幂，至少为请求的数量
    Assertions::assert_true(result.count >= 7);
    Assertions::assert_true(util::pow_of_2(result.count)); // 是2的幂

    // 可以安全使用整个分配的区域
    for (std::size_t i = 0; i < result.count; ++i) {
        alloc.construct(result.ptr + i, static_cast<i32>(i));
    }

    // 验证构造
    for (std::size_t i = 0; i < 7; ++i) {
        Assertions::assert_equals(static_cast<i32>(i), result.ptr[i]);
    }

    alloc.destroy_n(result.ptr, result.count);
    alloc.deallocate(result.ptr, result.count);

    // 边界情况：请求0
    result = alloc.allocate_at_least(0);
    Assertions::assert_null(result.ptr);
    Assertions::assert_equals(0ULL, result.count);
}

/**
 * @brief 异常安全测试
 */
void test_exception_safety() {
    Alloc<ThrowingConstructor> alloc;

    ThrowingConstructor::reset();

    constexpr std::size_t N = 5;
    auto* p = alloc.allocate(N);

    try {
        alloc.construct_n(p, N);
        Assertions::fail2("Expected exception not thrown");
    } catch (const std::runtime_error&) {
        Assertions::assert_equals(2, ThrowingConstructor::constructed);
        Assertions::assert_equals(0, ThrowingConstructor::alive);
        alloc.deallocate(p, N);
    }
}

// ==================== STL 容器集成测试 ====================

/**
 * @brief 与 std::vector 集成测试
 */
void test_vector_integration() {
    Alloc<i32> alloc;
    std::vector<i32, Alloc<i32>> vec(alloc);

    // 基本操作
    for (i32 i = 0; i < 100; ++i) {
        vec.push_back(i * 2);
    }

    Assertions::assert_equals(100ULL, vec.size());

    for (i32 i = 0; i < 100; ++i) {
        Assertions::assert_equals(i * 2, vec[i]);
    }

    // 测试重新分配内存（应该使用我们的分配器）
    vec.reserve(200);
    Assertions::assert_true(vec.capacity() >= 200);

    // 验证元素仍然正确
    for (i32 i = 0; i < 100; ++i) {
        Assertions::assert_equals(i * 2, vec[i]);
    }
}

/**
 * @brief 与 std::list 集成测试
 */
void test_list_integration() {
    Alloc<double> alloc;
    std::list<double, Alloc<double>> lst(alloc);

    // 添加元素
    for (i32 i = 0; i < 50; ++i) {
        lst.push_back(i * 1.5);
    }

    Assertions::assert_equals(50ULL, lst.size());

    // 验证元素
    i32 i = 0;
    for (const auto& val : lst) {
        Assertions::assertEquals(i * 1.5, val); // TODO 为 assert_equals 增加浮点支持
        ++i;
    }
}

/**
 * @brief 容器拷贝测试（分配器传播）
 */
void test_container_copy() {
    Alloc<std::string> alloc1;
    std::vector<std::string, Alloc<std::string>> vec1(alloc1);

    vec1.push_back("Hello");
    vec1.push_back("World");
    vec1.push_back("Test");

    // 拷贝构造函数应该传播分配器
    auto vec2 = vec1; // 应该使用相同的分配器

    Assertions::assert_equals(vec1.size(), vec2.size());
    for (std::size_t i = 0; i < vec1.size(); ++i) {
        Assertions::assert_equals(vec1[i], vec2[i]);
    }
}

// ==================== 性能测试 ====================

/**
 * @brief 批量分配性能测试
 */
void test_batch_allocation_performance() {
    Alloc<i32> alloc;
    constexpr std::size_t BATCH_SIZE = 1000;
    constexpr std::size_t NUM_BATCHES = 100;

    std::vector<i32*> pointers;
    pointers.reserve(NUM_BATCHES);

    // 批量分配
    for (std::size_t i = 0; i < NUM_BATCHES; ++i) {
        i32* ptr = alloc.allocate(BATCH_SIZE);
        Assertions::assert_not_null(ptr);

        // 批量构造
        for (std::size_t j = 0; j < BATCH_SIZE; ++j) {
            alloc.construct(ptr + j, static_cast<i32>(i * BATCH_SIZE + j));
        }

        pointers.push_back(ptr);
    }

    // 验证并释放
    for (std::size_t i = 0; i < NUM_BATCHES; ++i) {
        // 验证
        for (std::size_t j = 0; j < BATCH_SIZE; ++j) {
            Assertions::assert_equals(static_cast<i32>(i * BATCH_SIZE + j), pointers[i][j]);
        }

        alloc.destroy_n(pointers[i], BATCH_SIZE);
        alloc.deallocate(pointers[i], BATCH_SIZE);
    }
}

// ==================== 边界情况测试 ====================

/**
 * @brief 最大分配测试
 */
void test_max_allocation() {
    Alloc<char> alloc;

    Assertions::assert_throws<std::bad_alloc>([&]() {
        auto ptr = alloc.allocate(std::numeric_limits<std::size_t>::max());
    });
}

/**
 * @brief 混合操作测试
 */
void test_mixed_operations() {
    Alloc<i32> alloc;

    // 混合不同大小的分配
    i32* small = alloc.allocate(1);
    i32* medium = alloc.allocate(100);
    i32* large = alloc.allocate(1000);

    Assertions::assert_not_null(small);
    Assertions::assert_not_null(medium);
    Assertions::assert_not_null(large);

    // 交错构造
    alloc.construct(small, 42);
    for (i32 i = 0; i < 100; ++i) {
        alloc.construct(medium + i, i);
    }
    for (i32 i = 0; i < 1000; ++i) {
        alloc.construct(large + i, i * 2);
    }

    // 交错销毁和释放
    alloc.destroy(small);
    alloc.deallocate(small, 1);

    alloc.destroy_n(medium, 100);
    alloc.deallocate(medium, 100);

    alloc.destroy_n(large, 1000);
    alloc.deallocate(large, 1000);
}

// ==================== 运行所有测试 ====================

void test_allocator() {
    UnitTestGroup group("test_allocator");

    // 基础功能测试
    group.addTest("test_basic_allocation", test_basic_allocation);
    group.addTest("test_object_construction", test_object_construction);
    group.addTest("test_batch_operations", test_batch_operations);
    group.addTest("test_safe_creation", test_safe_creation);

    // 高级功能测试
    group.addTest("test_aligned_allocation", test_aligned_allocation);
    group.addTest("test_over_allocation", test_over_allocation);
    group.addTest("test_exception_safety", test_exception_safety);

    // STL集成测试
    group.addTest("test_vector_integration", test_vector_integration);
    group.addTest("test_list_integration", test_list_integration);
    group.addTest("test_container_copy", test_container_copy);

    // 性能测试
    group.addTest("test_batch_allocation_performance", test_batch_allocation_performance);

    // 边界情况测试
    group.addTest("test_max_allocation", test_max_allocation);
    group.addTest("test_mixed_operations", test_mixed_operations);

    group.startAll();
}

GROUP_NAME("test_allocator")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(test_basic_allocation),
    UNIT_TEST_ITEM(test_object_construction),
    UNIT_TEST_ITEM(test_batch_operations),
    UNIT_TEST_ITEM(test_safe_creation),
    UNIT_TEST_ITEM(test_aligned_allocation),
    UNIT_TEST_ITEM(test_over_allocation),
    UNIT_TEST_ITEM(test_exception_safety),
    UNIT_TEST_ITEM(test_vector_integration),
    UNIT_TEST_ITEM(test_list_integration),
    UNIT_TEST_ITEM(test_container_copy),
    UNIT_TEST_ITEM(test_batch_allocation_performance),
    UNIT_TEST_ITEM(test_max_allocation),
    UNIT_TEST_ITEM(test_mixed_operations))
} // namespace my::test::test_allocator