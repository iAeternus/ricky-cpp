#ifndef TEST_VEC_HPP
#define TEST_VEC_HPP

#include "ricky_test.hpp"
#include "vec.hpp"

namespace my::test::test_vec {

auto it_works = []() {
    util::Vec<i32> d(10, 0);
    util::Vec<util::Vec<i32>> d2(3, util::Vec<i32>(3, 0));
    util::Vec<i32> d3 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    Assertions::assertEquals("[0,0,0,0,0,0,0,0,0,0]"_cs, d.__str__());
    Assertions::assertEquals("[[0,0,0],[0,0,0],[0,0,0]]"_cs, d2.__str__());
    Assertions::assertEquals(1, d3.first());
    Assertions::assertEquals(10, d3.last());
};

auto should_append = []() {
    // Given
    usize n = 100;
    util::Vec<CString> d;

    // When
    for (usize i = 0; i < n; ++i) {
        d.push(cstr(i));
    }

    // Then
    Assertions::assertEquals(usize(n), d.size());
    Assertions::assertEquals(cstr(n - 1), d.at(d.size() - 1));

    i32 num = 0;
    for (const auto& it : d) {
        Assertions::assertEquals(cstr(num++), it);
    }
};

auto should_insert = []() {
    // Given
    util::Vec<i32> d = {1, 2, 3, 4, 5};

    // When
    d.insert(0, 100);

    // Then
    Assertions::assertEquals(6, d.size());
    Assertions::assertEquals(100, *d.begin());
    Assertions::assertEquals(1, d.find(1));
};

auto should_pop = []() {
    // Given
    util::Vec<i32> d = {1, 2, 3, 4, 5};

    // When
    d.pop();

    // Then
    Assertions::assertEquals(4, d.size());
    Assertions::assertEquals(4, d.at(d.size() - 1));

    // When
    d.pop(0);

    // Then
    Assertions::assertEquals(3, d.size());
    Assertions::assertEquals(2, d.at(0));
};

auto should_pop2 = []() {
    // Given
    util::Vec<i32> d;

    // When
    d.pop();

    // Then
    Assertions::assertEquals(0, d.size());
    Assertions::assertTrue(d.is_empty());
    Assertions::assertEquals(d.begin(), d.end());
};

auto should_clear = []() {
    // Given
    util::Vec<i32> d = {1, 2, 3, 4, 5};

    // When
    d.clear();

    // Then
    Assertions::assertTrue(d.is_empty());

    // When
    d.clear();

    // Then
    Assertions::assertEquals(0, d.size());
    Assertions::assertTrue(d.is_empty());
    Assertions::assertEquals(d.begin(), d.end());
};

auto should_swap = []() {
    // Given
    util::Vec<CString> d = {"aaa", "bbb", "ccc"};
    util::Vec<CString> d2 = {"ccc", "bbb", "aaa"};

    // When
    d.swap(d2);

    // Then
    Assertions::assertEquals("[ccc,bbb,aaa]"_cs, d.__str__());
    Assertions::assertEquals("[aaa,bbb,ccc]"_cs, d2.__str__());
};

auto should_to_array = []() {
    // Given
    util::Vec<i32> d = {1, 2, 3, 4, 5};

    // When
    auto arr = d.to_array();

    // Then
    Assertions::assertEquals(5, i32(arr.size()));
    Assertions::assertEquals(5, arr.at(arr.size() - 1));
    Assertions::assertEquals("[1,2,3,4,5]"_cs, arr.__str__());
};

auto should_slice = []() {
    // Given
    util::Vec<i32> v = {1, 2, 3, 4, 5, 6};

    // When
    auto res = v.slice(1ULL, 2LL);
    auto res2 = v.slice(3ULL);
    auto res3 = v.slice(1ULL, -1LL);

    // Then
    Assertions::assertEquals(1ULL, res.size());
    Assertions::assertEquals("[2]"_cs, res.__str__());
    Assertions::assertEquals("[4,5,6]"_cs, res2.__str__());
    Assertions::assertEquals("[2,3,4,5]"_cs, res3.__str__());
};

auto should_extend = []() {
    // Given
    util::Vec<i32> d = {1, 2, 3, 4, 5};

    // When
    d += {6, 7};
    d = d + util::Vec<i32>{8, 9, 10};

    // Then
    Assertions::assertEquals(10, d.size());
    Assertions::assertEquals("[1,2,3,4,5,6,7,8,9,10]"_cs, d.__str__());
};

auto should_at = []() {
    // Given
    util::Vec<char> d = {'a', 'b', 'c'};

    // When
    auto res = d.at(0);
    auto res2 = d.at(1);
    auto res3 = d.at(2);

    // Then
    Assertions::assertEquals('a', res);
    Assertions::assertEquals('b', res2);
    Assertions::assertEquals('c', res3);
};

auto should_find = []() {
    // Given
    util::Vec<CString> d = {"aaa"_cs, "bbb"_cs, "ccc"_cs};

    // When
    auto res = d.find("aaa"_cs);
    auto res2 = d.find("ddd"_cs);

    // Then
    Assertions::assertEquals(0, res);
    Assertions::assertEquals(d.size(), res2);
};

auto should_sort = []() {
    // Given
    util::Vec<i32> d = {6, 5, 7, 4, 8, 3, 9, 2, 0, 1};

    // When
    std::sort(d.begin(), d.end()); // TODO clangd 误报

    // Then
    Assertions::assertEquals("[0,1,2,3,4,5,6,7,8,9]"_cs, d.__str__());
};

auto test_opt = []() {
    // Given
    util::Vec<i32> v = {6, 5, 7, 4, 8, 3, 9, 2, 0, 1};

    // When
    auto res = util::opt<i32>(v, 2);

    // Then
    Assertions::assertEquals(7, res);
};

auto should_fail_to_opt_if_index_out_of_bounds = []() {
    // Given
    util::Vec<i32> v = {6, 5, 7, 4, 8, 3, 9, 2, 0, 1};

    // When & Then
    Assertions::assertThrows("Index 10 out of bounds [0..10] in opt function.", [&]() {
        util::opt<i32>(v, 10);
    });
};

auto should_fail_to_opt_if_type_mismatch = []() {
    // Given
    util::Vec<i32> v = {6, 5, 7, 4, 8, 3, 9, 2, 0, 1};

    // When & Then
    Assertions::assertThrows("type mismatch in opt function: expected[d], got[i]", [&]() {
        util::opt<f64>(v, 9);
    });
};

auto test_vec() {
    UnitTestGroup group{"test_vec"};

    group.addTest("it_works", it_works);
    group.addTest("should_append", should_append);
    group.addTest("should_insert", should_insert);
    group.addTest("should_pop", should_pop);
    group.addTest("should_pop2", should_pop2);
    group.addTest("should_clear", should_clear);
    group.addTest("should_swap", should_swap);
    group.addTest("should_to_array", should_to_array);
    group.addTest("should_slice", should_slice);
    group.addTest("should_extend", should_extend);
    group.addTest("should_at", should_at);
    group.addTest("should_find", should_find);
    group.addTest("should_sort", should_sort);
    group.addTest("test_opt", test_opt);
    group.addTest("should_fail_to_opt_if_index_out_of_bounds", should_fail_to_opt_if_index_out_of_bounds);
    group.addTest("should_fail_to_opt_if_type_mismatch", should_fail_to_opt_if_type_mismatch);

    group.startAll();
}

constexpr i32 N = 1e6;

auto speed_of_util_vec_append_string = []() {
    util::Vec<std::string> d;
    for (usize i = 0; i < N; ++i) {
        d.push("aaaaa");
    }
};

auto speed_of_std_vector_push_back_string = []() {
    std::vector<std::string> v;
    for (usize i = 0; i < N; ++i) {
        v.push_back("aaaaa");
    }
};

auto speed_of_util_vec_append_i32 = []() {
    util::Vec<i32> d;
    for (usize i = 0; i < N; ++i) {
        d.push(i);
    }
};

auto speed_of_std_vector_push_back_i32 = []() {
    std::vector<i32> v;
    for (usize i = 0; i < N; ++i) {
        v.push_back(i);
    }
};

auto test_vec_speed() {
    UnitTestGroup group{"test_vec_speed"};

    group.addTest("speed_of_util_vec_append_string", speed_of_util_vec_append_string);
    group.addTest("speed_of_std_vector_push_back_string", speed_of_std_vector_push_back_string);
    group.addTest("speed_of_util_vec_append_i32", speed_of_util_vec_append_i32);
    group.addTest("speed_of_std_vector_push_back_i32", speed_of_std_vector_push_back_i32);

    group.startAll();
}

} // namespace my::test::test_vec

#endif // TEST_VEC_HPP