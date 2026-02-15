#include "test_vec.hpp"
#include "vec.hpp"
#include "ricky_test.hpp"

namespace my::test::test_vec {

void it_works() {
    util::Vec<i32> d(10, 0);
    util::Vec<util::Vec<i32>> d2(3, util::Vec<i32>(3, 0));
    util::Vec<i32> d3 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    Assertions::assertEquals("[0,0,0,0,0,0,0,0,0,0]"_cs, d.__str__());
    Assertions::assertEquals("[[0,0,0],[0,0,0],[0,0,0]]"_cs, d2.__str__());
    Assertions::assertEquals(1, d3.first());
    Assertions::assertEquals(10, d3.last());
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
    Assertions::assertEquals(usize(n), d.len());
    Assertions::assertEquals(cstr(n - 1), d.at(d.len() - 1));

    i32 num = 0;
    for (const auto& it : d) {
        Assertions::assertEquals(cstr(num++), it);
    }
}

void should_insert() {
    // Given
    util::Vec<i32> d = {1, 2, 3, 4, 5};

    // When
    d.insert(0, 100);

    // Then
    Assertions::assertEquals(6, d.len());
    Assertions::assertEquals(100, *d.begin());
    Assertions::assertEquals(1, d.find(1));
}

void should_pop() {
    // Given
    util::Vec<i32> d = {1, 2, 3, 4, 5};

    // When
    d.pop();

    // Then
    Assertions::assertEquals(4, d.len());
    Assertions::assertEquals(4, d.at(d.len() - 1));

    // When
    d.pop(0);

    // Then
    Assertions::assertEquals(3, d.len());
    Assertions::assertEquals(2, d.at(0));
}

void should_pop2() {
    // Given
    util::Vec<i32> d;

    // When
    d.pop();

    // Then
    Assertions::assertEquals(0, d.len());
    Assertions::assertTrue(d.is_empty());
    Assertions::assertEquals(d.begin(), d.end());
}

void should_clear() {
    // Given
    util::Vec<i32> d = {1, 2, 3, 4, 5};

    // When
    d.clear();

    // Then
    Assertions::assertTrue(d.is_empty());

    // When
    d.clear();

    // Then
    Assertions::assertEquals(0, d.len());
    Assertions::assertTrue(d.is_empty());
    Assertions::assertEquals(d.begin(), d.end());
}

void should_swap() {
    // Given
    util::Vec<CString> d = {"aaa", "bbb", "ccc"};
    util::Vec<CString> d2 = {"ccc", "bbb", "aaa"};

    // When
    d.swap(d2);

    // Then
    Assertions::assertEquals("[ccc,bbb,aaa]"_cs, d.__str__());
    Assertions::assertEquals("[aaa,bbb,ccc]"_cs, d2.__str__());
}

void should_to_array() {
    // Given
    util::Vec<i32> d = {1, 2, 3, 4, 5};

    // When
    auto arr = d.to_array();

    // Then
    Assertions::assertEquals(5, i32(arr.len()));
    Assertions::assertEquals(5, arr.at(arr.len() - 1));
    Assertions::assertEquals("[1,2,3,4,5]"_cs, arr.__str__());
}

void should_slice() {
    // Given
    util::Vec<i32> v = {1, 2, 3, 4, 5, 6};

    // When
    auto res = v.slice(1ULL, 2LL);
    auto res2 = v.slice(3ULL);
    auto res3 = v.slice(1ULL, -1LL);

    // Then
    Assertions::assertEquals(1ULL, res.len());
    Assertions::assertEquals("[2]"_cs, res.__str__());
    Assertions::assertEquals("[4,5,6]"_cs, res2.__str__());
    Assertions::assertEquals("[2,3,4,5]"_cs, res3.__str__());
}

void should_extend() {
    // Given
    util::Vec<i32> d = {1, 2, 3, 4, 5};

    // When
    d += {6, 7};
    d = d + util::Vec<i32>{8, 9, 10};

    // Then
    Assertions::assertEquals(10, d.len());
    Assertions::assertEquals("[1,2,3,4,5,6,7,8,9,10]"_cs, d.__str__());
}

void should_at() {
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
}

void should_find() {
    // Given
    util::Vec<CString> d = {"aaa"_cs, "bbb"_cs, "ccc"_cs};

    // When
    auto res = d.find("aaa"_cs);
    auto res2 = d.find("ddd"_cs);

    // Then
    Assertions::assertEquals(0, res);
    Assertions::assertEquals(d.len(), res2);
}

void should_sort() {
    // Given
    util::Vec<i32> d = {6, 5, 7, 4, 8, 3, 9, 2, 0, 1};

    // When
    std::sort(d.begin(), d.end()); // TODO clangd 误报

    // Then
    Assertions::assertEquals("[0,1,2,3,4,5,6,7,8,9]"_cs, d.__str__());
}

void test_opt() {
    // Given
    util::Vec<i32> v = {6, 5, 7, 4, 8, 3, 9, 2, 0, 1};

    // When
    auto res = util::opt<i32>(v, 2);

    // Then
    Assertions::assertEquals(7, res);
}

void should_fail_to_opt_if_index_out_of_bounds() {
    // Given
    util::Vec<i32> v = {6, 5, 7, 4, 8, 3, 9, 2, 0, 1};

    // When & Then
    Assertions::assertThrows("Index 10 out of bounds [0..10] in opt function.", [&]() {
        util::opt<i32>(v, 10);
    });
}

void should_fail_to_opt_if_type_mismatch() {
    // Given
    util::Vec<i32> v = {6, 5, 7, 4, 8, 3, 9, 2, 0, 1};

    // When & Then
    Assertions::assertThrows("type mismatch in opt function: expected[d], got[i]", [&]() {
        util::opt<f64>(v, 9);
    });
}

constexpr i32 N = 1e6;

void speed_of_util_vec_append_string() {
    util::Vec<std::string> d;
    for (usize i = 0; i < N; ++i) {
        d.push("aaaaa");
    }
}

void speed_of_std_vector_push_back_string() {
    std::vector<std::string> v;
    for (usize i = 0; i < N; ++i) {
        v.push_back("aaaaa");
    }
}

void speed_of_util_vec_append_i32() {
    util::Vec<i32> d;
    for (usize i = 0; i < N; ++i) {
        d.push(i);
    }
}

void speed_of_std_vector_push_back_i32() {
    std::vector<i32> v;
    for (usize i = 0; i < N; ++i) {
        v.push_back(i);
    }
}

void test_vec_speed() {
    speed_of_util_vec_append_string();
    speed_of_std_vector_push_back_string();
    speed_of_util_vec_append_i32();
    speed_of_std_vector_push_back_i32();
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
    UNIT_TEST_ITEM(should_fail_to_opt_if_type_mismatch))

} // namespace my::test::test_vec