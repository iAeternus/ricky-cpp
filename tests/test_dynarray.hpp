#ifndef TEST_DYNARRAY_HPP
#define TEST_DYNARRAY_HPP

#include "ricky_test.hpp"
#include "DynArray.hpp"
#include "Vec.hpp"

namespace my::test::test_dynarray {

auto it_works = []() {
    util::DynArray<i32> d(10, 0);
    util::DynArray<util::DynArray<i32>> d2(3, util::DynArray<i32>(3, 0));
    util::DynArray<i32> d3 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    Assertions::assertEquals("[0,0,0,0,0,0,0,0,0,0]"_cs, d.__str__());
    Assertions::assertEquals("[[0,0,0],[0,0,0],[0,0,0]]"_cs, d2.__str__());
    Assertions::assertEquals(1, d3.front());
    Assertions::assertEquals(10, d3.back());
};

auto should_append = []() {
    // Given
    i32 n = 100;
    util::DynArray<CString> d;

    // When
    for (i32 i = 0; i < n; ++i) {
        d.append(cstr(i));
    }

    // Then
    Assertions::assertEquals(n, d.size());
    Assertions::assertEquals(cstr(n - 1), d.at(d.size() - 1));

    i32 num = 0;
    for (const auto& it : d) {
        Assertions::assertEquals(cstr(num++), it);
    }
};

auto should_insert = []() {
    // Given
    util::DynArray<i32> d = {1, 2, 3, 4, 5};

    // When
    d.insert(0, 100);

    // Then
    Assertions::assertEquals(6, d.size());
    Assertions::assertEquals(100, *d.begin());
    Assertions::assertEquals(1, d.find(1));
};

auto should_pop = []() {
    // Given
    util::DynArray<i32> d = {1, 2, 3, 4, 5};

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
    util::DynArray<i32> d;

    // When
    d.pop();

    // Then
    Assertions::assertEquals(0, d.size());
    Assertions::assertTrue(d.empty());
    Assertions::assertEquals(d.begin(), d.end());
};

auto should_clear = []() {
    // Given
    util::DynArray<i32> d = {1, 2, 3, 4, 5};

    // When
    d.clear();

    // Then
    Assertions::assertTrue(d.empty());

    // When
    d.clear();

    // Then
    Assertions::assertEquals(0, d.size());
    Assertions::assertTrue(d.empty());
    Assertions::assertEquals(d.begin(), d.end());
};

auto should_to_array = []() {
    // Given
    util::DynArray<i32> d = {1, 2, 3, 4, 5};

    // When
    auto arr = d.to_array();

    // Then
    Assertions::assertEquals(5, i32(arr.size()));
    Assertions::assertEquals(5, arr.at(arr.size() - 1));
    Assertions::assertEquals("[1,2,3,4,5]"_cs, arr.__str__());
};

auto should_extend = []() {
    // Given
    util::DynArray<i32> d = {1, 2, 3, 4, 5};

    // When
    d += {6, 7};
    d = d + util::DynArray<i32>{8, 9, 10};

    // Then
    Assertions::assertEquals(10, d.size());
    Assertions::assertEquals("[1,2,3,4,5,6,7,8,9,10]"_cs, d.__str__());
};

auto should_at = []() {
    // Given
    util::DynArray<char> d = {'a', 'b', 'c'};

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
    util::DynArray<CString> d = {"aaa"_cs, "bbb"_cs, "ccc"_cs};

    // When
    auto res = d.find("aaa"_cs);
    auto res2 = d.find("ddd"_cs);

    // Then
    Assertions::assertEquals(0, res);
    Assertions::assertEquals(d.size(), res2);
};

// auto should_sort = []() {
//     // Given
//     util::DynArray<i32> d = {6, 5, 7, 4, 8, 3, 9, 2, 0, 1};

//     // When
//     std::sort(d.begin(), d.end());

//     // Then
//     io::println(d);
//     Assertions::assertEquals("[0,1,2,3,4,5,6,7,8,9]"_cs, d.__str__());
// };

inline void test_dynarray() {
    UnitTestGroup group("test_dynarray");

    group.addTest("it_works", it_works);
    group.addTest("should_append", should_append);
    group.addTest("should_insert", should_insert);
    group.addTest("should_pop", should_pop);
    group.addTest("should_pop2", should_pop2);
    group.addTest("should_clear", should_clear);
    group.addTest("should_to_array", should_to_array);
    group.addTest("should_extend", should_extend);
    group.addTest("should_at", should_at);
    group.addTest("should_find", should_find);
    // group.addTest("should_sort", should_sort);

    group.startAll();
}

constexpr i32 N = 1e6;

auto speed_of_dny_array_append_string = []() {
    util::DynArray<std::string> d;
    for (i32 i = 0; i < N; ++i) {
        d.append("aaaaa");
    }
};

auto speed_of_util_vector_append_string = []() {
    util::Vec<std::string> d;
    for (i32 i = 0; i < N; ++i) {
        d.append("aaaaa");
    }
};

auto speed_of_vector_push_back_string = []() {
    std::vector<std::string> v;
    for (i32 i = 0; i < N; ++i) {
        v.push_back("aaaaa");
    }
};

auto speed_of_dny_array_append_i32 = []() {
    util::DynArray<i32> d;
    for (i32 i = 0; i < N; ++i) {
        d.append(i);
    }
};

auto speed_of_util_vector_append_i32 = []() {
    util::Vec<i32> d;
    for (i32 i = 0; i < N; ++i) {
        d.append(i);
    }
};

auto speed_of_vector_push_back_i32 = []() {
    std::vector<i32> v;
    for (i32 i = 0; i < N; ++i) {
        v.push_back(i);
    }
};

inline void test_dynarray_speed() {
    UnitTestGroup group{"test_dynarray_speed"};

    group.addTest("speed_of_dny_array_append_string", speed_of_dny_array_append_string);
    group.addTest("speed_of_util_vector_append_string", speed_of_util_vector_append_string);
    group.addTest("speed_of_vector_push_back_string", speed_of_vector_push_back_string);
    group.addTest("speed_of_dny_array_append_i32", speed_of_dny_array_append_i32);
    group.addTest("speed_of_util_vector_append_i32", speed_of_util_vector_append_i32);
    group.addTest("speed_of_vector_push_back_i32", speed_of_vector_push_back_i32);

    group.startAll();
}

} // namespace my::test::test_dynarray

#endif // TEST_DYNARRAY_HPP