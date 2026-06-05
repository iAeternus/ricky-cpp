#include "test_dynarray.hpp"
#include "dyn_array.hpp"
#include "ricky_test.hpp"

namespace my::test::test_dynarray {

void it_works() {
    util::DynArray<i32> d(10, 0);
    util::DynArray<util::DynArray<i32>> d2(3, util::DynArray<i32>(3, 0));
    util::DynArray<i32> d3 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    Assertions::assert_equals("[0,0,0,0,0,0,0,0,0,0]"_cs, d.to_string());
    Assertions::assert_equals("[[0,0,0],[0,0,0],[0,0,0]]"_cs, d2.to_string());
    Assertions::assert_equals(1, d3.front());
    Assertions::assert_equals(10, d3.back());
}

void should_append() {
    // Given
    usize n = 100;
    util::DynArray<CString> d;

    // When
    for (usize i = 0; i < n; ++i) {
        d.append(cstr(i));
    }

    // Then
    Assertions::assert_equals(n, d.len());
    Assertions::assert_equals(cstr(n - 1), d.at(d.len() - 1));

    i32 num = 0;
    for (const auto& it : d) {
        Assertions::assert_equals(cstr(num++), it);
    }
}

void should_insert() {
    // Given
    util::DynArray<i32> d = {1, 2, 3, 4, 5};

    // When
    d.insert(0, 100);

    // Then
    Assertions::assert_equals(6ULL, d.len());
    Assertions::assert_equals(100, *d.begin());
    Assertions::assert_equals(1ULL, d.find(1));
}

void should_pop() {
    // Given
    util::DynArray<i32> d = {1, 2, 3, 4, 5};

    // When
    d.pop();

    // Then
    Assertions::assert_equals(4ULL, d.len());
    Assertions::assert_equals(4, d.at(d.len() - 1));

    // When
    d.pop(0);

    // Then
    Assertions::assert_equals(3ULL, d.len());
    Assertions::assert_equals(2, d.at(0));
}

void should_pop2() {
    // Given
    util::DynArray<i32> d;

    // When
    d.pop();

    // Then
    Assertions::assert_equals(0ULL, d.len());
    Assertions::assert_true(d.empty());
    Assertions::assert_equals(d.begin(), d.end());
}

void should_clear() {
    // Given
    util::DynArray<i32> d = {1, 2, 3, 4, 5};

    // When
    d.clear();

    // Then
    Assertions::assert_true(d.empty());

    // When
    d.clear();

    // Then
    Assertions::assert_equals(0ULL, d.len());
    Assertions::assert_true(d.empty());
    Assertions::assert_equals(d.begin(), d.end());
}

void should_to_array() {
    // Given
    util::DynArray<i32> d = {1, 2, 3, 4, 5};

    // When
    auto arr = d.to_array();

    // Then
    Assertions::assert_equals(5ULL, arr.len());
    Assertions::assert_equals(5, arr.at(arr.len() - 1));
    Assertions::assert_equals("[1,2,3,4,5]"_cs, arr.to_string());
}

void should_extend() {
    // Given
    util::DynArray<i32> d = {1, 2, 3, 4, 5};

    // When
    d += {6, 7};
    d = d + util::DynArray<i32>{8, 9, 10};

    // Then
    Assertions::assert_equals(10ULL, d.len());
    Assertions::assert_equals("[1,2,3,4,5,6,7,8,9,10]"_cs, d.to_string());
}

void should_at() {
    // Given
    util::DynArray<char> d = {'a', 'b', 'c'};

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
    util::DynArray<CString> d = {"aaa", "bbb", "ccc"_cs};

    // When
    auto res = d.find("aaa"_cs);
    auto res2 = d.find("ddd"_cs);

    // Then
    Assertions::assert_equals(0ULL, res);
    Assertions::assert_equals(d.len(), res2);
}

// auto should_sort = []() {
//     // Given
//     util::DynArray<i32> d = {6, 5, 7, 4, 8, 3, 9, 2, 0, 1};

//     // When
//     std::sort(d.begin(), d.end());

//     // Then
//     io::println(d);
//     Assertions::assert_equals("[0,1,2,3,4,5,6,7,8,9]"_cs, d.to_string());
// };

GROUP_NAME("test_dynarray")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(it_works),
    UNIT_TEST_ITEM(should_append),
    UNIT_TEST_ITEM(should_insert),
    UNIT_TEST_ITEM(should_pop),
    UNIT_TEST_ITEM(should_pop2),
    UNIT_TEST_ITEM(should_clear),
    UNIT_TEST_ITEM(should_to_array),
    UNIT_TEST_ITEM(should_extend),
    UNIT_TEST_ITEM(should_at),
    UNIT_TEST_ITEM(should_find))

} // namespace my::test::test_dynarray
