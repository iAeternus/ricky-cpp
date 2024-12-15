#ifndef TEST_DYNARRAY_HPP
#define TEST_DYNARRAY_HPP

#include "ricky_test.hpp"
#include "DynArray.hpp"

namespace my::test::test_dynarray {

auto should_append = []() {
    // Given
    int n = 100;
    util::DynArray<CString> d;

    // When
    for (int i = 0; i < n; ++i) {
        d.append(cstr(i));
    }

    // Then
    Assertions::assertEquals(n, int(d.size()));
    Assertions::assertEquals(cstr(n - 1), d.at(d.size() - 1));

    int num = 0;
    for (const auto& it : d) {
        Assertions::assertEquals(cstr(num++), it);
    }
};

auto should_insert = []() {
    // Given
    util::DynArray<int> d = {1, 2, 3, 4, 5};

    // When
    d.insert(0, 100);

    // Then
    Assertions::assertEquals(6, int(d.size()));
    Assertions::assertEquals(100, *d.begin());
};

auto should_pop = []() {
    // Given
    util::DynArray<int> d = {1, 2, 3, 4, 5};

    // When
    d.pop();

    // Then
    Assertions::assertEquals(4, int(d.size()));
    Assertions::assertEquals(4, d.at(d.size() - 1));

    // When
    d.pop(0);

    // Then
    Assertions::assertEquals(3, int(d.size()));
    Assertions::assertEquals(2, d.at(0));
};

auto should_pop2 = []() {
    // Given
    util::DynArray<int> d;

    // When
    d.pop();

    // Then
    Assertions::assertEquals(0, int(d.size()));
    Assertions::assertTrue(d.empty());
    Assertions::assertEquals(d.begin(), d.end());
};

auto should_clear = []() {
    // Given
    util::DynArray<int> d = {1, 2, 3, 4, 5};

    // When
    d.clear();

    // Then
    Assertions::assertTrue(d.empty());

    // When
    d.clear();

    // Then
    Assertions::assertEquals(0, int(d.size()));
    Assertions::assertTrue(d.empty());
    Assertions::assertEquals(d.begin(), d.end());
};

auto should_to_array = []() {
    // Given
    util::DynArray<int> d = {1, 2, 3, 4, 5};

    // When
    auto arr = d.toArray();

    // Then
    Assertions::assertEquals(5, int(arr.size()));
    Assertions::assertEquals(5, arr.at(arr.size() - 1));
    Assertions::assertEquals(CString{"[1,2,3,4,5]"}, arr.__str__());
};

auto should_extend = []() {
    // Given
    util::DynArray<int> d = {1, 2, 3, 4, 5};

    // When
    d += {6, 7};
    d = d + util::DynArray<int>{8, 9, 10};

    // Then
    Assertions::assertEquals(10, int(d.size()));
    Assertions::assertEquals(CString{"[1,2,3,4,5,6,7,8,9,10]"}, d.__str__());
};

void test_dynarray() {
    UnitTestGroup group("test_dynarray");

    group.addTest("should_append", should_append);
    group.addTest("should_insert", should_insert);
    group.addTest("should_pop", should_pop);
    group.addTest("should_pop2", should_pop2);
    group.addTest("should_clear", should_clear);
    group.addTest("should_to_array", should_to_array);
    group.addTest("should_extend", should_extend);

    group.startAll();
}

constexpr int N = 1e6;

auto speed_of_dny_array_append_string = []() {
    util::DynArray<std::string> d;
    for(int i = 0; i < N; ++i) {
        d.append("aaaaa");
    }
};

auto speed_of_vector_push_back_string = []() {
    std::vector<std::string> v;
    for(int i = 0; i < N; ++i) {
        v.push_back("aaaaa");
    }
};

auto speed_of_dny_array_append_int = []() {
    util::DynArray<int> d;
    for(int i = 0; i < N; ++i) {
        d.append(i);
    }
};

auto speed_of_vector_push_back_int = []() {
    std::vector<int> v;
    for(int i = 0; i < N; ++i) {
        v.push_back(i);
    }
};

void test_dynarray_speed() {
    UnitTestGroup group("test_dynarray_speed");

    group.addTest("speed_of_dny_array_append_string", speed_of_dny_array_append_string);
    group.addTest("speed_of_vector_push_back_string", speed_of_vector_push_back_string);
    group.addTest("speed_of_dny_array_append_int", speed_of_dny_array_append_int);
    group.addTest("speed_of_vector_push_back_int", speed_of_vector_push_back_int);

    group.startAll();
}

} // namespace my::test::test_dynarray

#endif // TEST_DYNARRAY_HPP