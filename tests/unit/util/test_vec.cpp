#include "test_vec.hpp"
#include "vec.hpp"
#include "ricky_test.hpp"

namespace my::test::test_vec {

void it_works() {
    util::Vec<i32> d(10, 0);
    util::Vec<util::Vec<i32>> d2(3, util::Vec<i32>(3, 0));
    util::Vec<i32> d3 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    Assertions::assert_equals("[0,0,0,0,0,0,0,0,0,0]"_cs, d.to_string());
    Assertions::assert_equals("[[0,0,0],[0,0,0],[0,0,0]]"_cs, d2.to_string());
    Assertions::assert_equals(1, d3.first());
    Assertions::assert_equals(10, d3.last());
}

void should_append() {
    // Given
    usize n = 100;
    util::Vec<CString> d;

    // When
    for (usize i = 0; i < n; ++i) {
        d.push(cstr(i));
    }

    // Then
    Assertions::assert_equals(usize(n), d.len());
    Assertions::assert_equals(cstr(n - 1), d.at(d.len() - 1));

    i32 num = 0;
    for (const auto& it : d) {
        Assertions::assert_equals(cstr(num++), it);
    }
}

void should_insert() {
    // Given
    util::Vec<i32> d = {1, 2, 3, 4};

    // When
    d.insert(4, 200);
    d.insert(0, 100);

    // Then
    Assertions::assert_equals(6, d.len());
    Assertions::assert_equals(100, *d.begin());
    Assertions::assert_equals(200, d[d.len() - 1]);
    Assertions::assert_equals(0, d.find(100));
    Assertions::assert_equals(5, d.find(200));
}

void should_pop() {
    // Given
    util::Vec<i32> d = {1, 2, 3, 4, 5};

    // When
    d.pop();

    // Then
    Assertions::assert_equals(4, d.len());
    Assertions::assert_equals(4, d.at(d.len() - 1));

    // When
    d.pop(0);

    // Then
    Assertions::assert_equals(3, d.len());
    Assertions::assert_equals(2, d.at(0));
}

void should_pop2() {
    // Given
    util::Vec<i32> d;

    // When
    d.pop();

    // Then
    Assertions::assert_equals(0, d.len());
    Assertions::assert_true(d.is_empty());
    Assertions::assert_equals(d.begin(), d.end());
}

void should_clear() {
    // Given
    util::Vec<i32> d = {1, 2, 3, 4, 5};

    // When
    d.clear();

    // Then
    Assertions::assert_true(d.is_empty());

    // When
    d.clear();

    // Then
    Assertions::assert_equals(0, d.len());
    Assertions::assert_true(d.is_empty());
    Assertions::assert_equals(d.begin(), d.end());
}

void should_swap() {
    // Given
    util::Vec<CString> d = {"aaa", "bbb", "ccc"};
    util::Vec<CString> d2 = {"ccc", "bbb", "aaa"};

    // When
    d.swap(d2);

    // Then
    Assertions::assert_equals("[ccc,bbb,aaa]"_cs, d.to_string());
    Assertions::assert_equals("[aaa,bbb,ccc]"_cs, d2.to_string());
}

void should_to_array() {
    // Given
    util::Vec<i32> d = {1, 2, 3, 4, 5};

    // When
    auto arr = d.to_array();

    // Then
    Assertions::assert_equals(5, i32(arr.len()));
    Assertions::assert_equals(5, arr.at(arr.len() - 1));
    Assertions::assert_equals("[1,2,3,4,5]"_cs, arr.to_string());
}

void should_slice() {
    // Given
    util::Vec<i32> v = {1, 2, 3, 4, 5, 6};

    // When
    auto res = v.slice(1ULL, 2LL);
    auto res2 = v.slice(3ULL);
    auto res3 = v.slice(1ULL, -1LL);

    // Then
    Assertions::assert_equals(1ULL, res.len());
    Assertions::assert_equals("[2]"_cs, res.to_string());
    Assertions::assert_equals("[4,5,6]"_cs, res2.to_string());
    Assertions::assert_equals("[2,3,4,5]"_cs, res3.to_string());
}

void should_extend() {
    // Given
    util::Vec<i32> d = {1, 2, 3, 4, 5};

    // When
    d += {6, 7};
    d = d + util::Vec<i32>{8, 9, 10};

    // Then
    Assertions::assert_equals(10, d.len());
    Assertions::assert_equals("[1,2,3,4,5,6,7,8,9,10]"_cs, d.to_string());
}

void should_at() {
    // Given
    util::Vec<char> d = {'a', 'b', 'c'};

    // When
    auto res = d.at(0);
    auto res2 = d.at(1);
    auto res3 = d.at(2);

    // Then
    Assertions::assert_equals('a', res);
    Assertions::assert_equals('b', res2);
    Assertions::assert_equals('c', res3);
}

void should_find() {
    // Given
    util::Vec<CString> d = {"aaa"_cs, "bbb"_cs, "ccc"_cs};

    // When
    auto res = d.find("aaa"_cs);
    auto res2 = d.find("ddd"_cs);

    // Then
    Assertions::assert_equals(0, res);
    Assertions::assert_equals(d.len(), res2);
}

void should_sort() {
    // Given
    util::Vec<i32> d = {6, 5, 7, 4, 8, 3, 9, 2, 0, 1};

    // When
    std::sort(d.begin(), d.end()); // TODO 说明

    // Then
    Assertions::assert_equals("[0,1,2,3,4,5,6,7,8,9]"_cs, d.to_string());
}

void test_opt() {
    // Given
    util::Vec<i32> v = {6, 5, 7, 4, 8, 3, 9, 2, 0, 1};

    // When
    auto res = util::opt<i32>(v, 2);

    // Then
    Assertions::assert_equals(7, res);
}

void should_fail_to_opt_if_index_out_of_bounds() {
    // Given
    util::Vec<i32> v = {6, 5, 7, 4, 8, 3, 9, 2, 0, 1};

    // When & Then
    Assertions::assert_throws<Exception>("Index 10 out of bounds [0..10] in opt function.", [&]() {
        util::opt<i32>(v, 10);
    });
}

void should_fail_to_opt_if_type_mismatch() {
    // Given
    util::Vec<i32> v = {6, 5, 7, 4, 8, 3, 9, 2, 0, 1};

    // When & Then
    Assertions::assert_throws<Exception>("type mismatch in opt function: expected[d], got[i]", [&]() {
        util::opt<f64>(v, 9);
    });
}

// === Vec 回归测试：迭代器构造函数 ===
// 修复：Vec(It first, It last) 之前未初始化 data_/len_/capacity_，
// 导致 push() 调用 try_expand() 时使用垃圾值，造成堆损坏。
// 修复后：len_(0), capacity_(0), data_(nullptr)，由 try_expand 按需分配。

void should_construct_from_iterator_range() {
    i32 arr[] = {10, 20, 30, 40, 50};
    util::Vec<i32> v(std::begin(arr), std::end(arr));

    Assertions::assert_equals(5, v.len());
    Assertions::assert_equals("[10,20,30,40,50]"_cs, v.to_string());
    Assertions::assert_equals(10, v[0]);
    Assertions::assert_equals(50, v[4]);
}

void should_push_after_iterator_construction() {
    i32 arr[] = {1, 2, 3};
    util::Vec<i32> v(std::begin(arr), std::end(arr));

    v.push(4);
    v.push(5);

    Assertions::assert_equals(5, v.len());
    Assertions::assert_equals("[1,2,3,4,5]"_cs, v.to_string());
}

void should_construct_from_empty_iterator_range() {
    i32 arr[] = {1, 2, 3};
    util::Vec<i32> v(std::begin(arr), std::begin(arr));

    Assertions::assert_true(v.is_empty());
    Assertions::assert_equals(0, v.len());

    v.push(42);
    Assertions::assert_equals(1, v.len());
    Assertions::assert_equals(42, v[0]);
}

void should_push_large_after_iterator_construction() {
    i32 arr[] = {1, 2, 3};
    util::Vec<i32> v(std::begin(arr), std::end(arr));

    for (i32 i = 0; i < 100; ++i) {
        v.push(i);
    }

    Assertions::assert_equals(103, v.len());
    Assertions::assert_equals(99, v.last());
    Assertions::assert_equals(1, v[0]);
    Assertions::assert_equals(2, v[1]);
    Assertions::assert_equals(3, v[2]);
}

void should_construct_from_iterator_range_with_nontrivial_type() {
    util::Vec<CString> src = {cstr("hello"), cstr("world"), cstr("vec")};
    util::Vec<CString> v(src.begin(), src.end());

    Assertions::assert_equals(3, v.len());
    Assertions::assert_equals("hello"_cs, v[0]);
    Assertions::assert_equals("world"_cs, v[1]);
    Assertions::assert_equals("vec"_cs, v[2]);

    v.push(cstr("regression"));
    Assertions::assert_equals(4, v.len());
    Assertions::assert_equals("regression"_cs, v[3]);
}

// === Vec 回归测试：resize ===
// 修复：resize 改用 alloc_.construct/destroy 替代 placement new + 手动析构，
// 且增加 data_ 空指针保护，确保 data_ 为 null 时跳过 memcpy/construct。

void should_resize_up_with_non_trivial_types() {
    util::Vec<CString> v = {cstr("a"), cstr("b"), cstr("c")};

    v.resize(10);

    Assertions::assert_equals(3, v.len());
    Assertions::assert_equals("a"_cs, v[0]);
    Assertions::assert_equals("b"_cs, v[1]);
    Assertions::assert_equals("c"_cs, v[2]);
    Assertions::assert_equals(10, v.capacity());
}

void should_resize_down_preserves_content() {
    util::Vec<i32> v = {1, 2, 3, 4, 5};

    v.resize(3);

    Assertions::assert_equals(3, v.len());
    Assertions::assert_equals("[1,2,3]"_cs, v.to_string());
    Assertions::assert_equals(3, v.capacity());
}

void should_handle_multiple_resizes() {
    util::Vec<i32> v = {1, 2, 3};

    v.resize(10);
    v.resize(5);
    v.resize(20);

    Assertions::assert_equals(3, v.len());
    Assertions::assert_equals("[1,2,3]"_cs, v.to_string());
    Assertions::assert_equals(20, v.capacity());

    v.push(4);
    Assertions::assert_equals(4, v.len());
    Assertions::assert_equals("[1,2,3,4]"_cs, v.to_string());
}

GROUP_NAME("test_vec")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(it_works),
    UNIT_TEST_ITEM(should_append),
    UNIT_TEST_ITEM(should_insert),
    UNIT_TEST_ITEM(should_pop),
    UNIT_TEST_ITEM(should_pop2),
    UNIT_TEST_ITEM(should_clear),
    UNIT_TEST_ITEM(should_swap),
    UNIT_TEST_ITEM(should_to_array),
    UNIT_TEST_ITEM(should_slice),
    UNIT_TEST_ITEM(should_extend),
    UNIT_TEST_ITEM(should_at),
    UNIT_TEST_ITEM(should_find),
    UNIT_TEST_ITEM(should_sort),
    UNIT_TEST_ITEM(test_opt),
    UNIT_TEST_ITEM(should_fail_to_opt_if_index_out_of_bounds),
    UNIT_TEST_ITEM(should_fail_to_opt_if_type_mismatch),
    // 回归测试
    UNIT_TEST_ITEM(should_construct_from_iterator_range),
    UNIT_TEST_ITEM(should_push_after_iterator_construction),
    UNIT_TEST_ITEM(should_construct_from_empty_iterator_range),
    UNIT_TEST_ITEM(should_push_large_after_iterator_construction),
    UNIT_TEST_ITEM(should_construct_from_iterator_range_with_nontrivial_type),
    UNIT_TEST_ITEM(should_resize_up_with_non_trivial_types),
    UNIT_TEST_ITEM(should_resize_down_preserves_content),
    UNIT_TEST_ITEM(should_handle_multiple_resizes))

} // namespace my::test::test_vec
