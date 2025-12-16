#ifndef TEST_CHAIN_HPP
#define TEST_CHAIN_HPP

#include "ricky_test.hpp"
#include "chain.hpp"

namespace my::test::test_chain {

auto should_append = []() {
    // Given
    usize N = 100;
    util::ChainList<CString> c;

    // When
    for (usize i = 0; i < N; ++i) {
        c.append(cstr(i));
    }

    // Then
    Assertions::assertEquals(N, c.size());
    Assertions::assertFalse(c.empty());
    Assertions::assertTrue(c.contains(cstr(N - 1)));
    Assertions::assertFalse(c.contains(cstr(N)));
    Assertions::assertEquals(cstr(N - 1), c[N - 1]);
};

auto should_iterable = []() {
    // Given
    usize N = 100;
    util::ChainList<i32> c;
    for (usize i = 0; i < N; ++i) {
        c.append(i + 1);
    }

    // When
    i32 sum = 0;
    for (const auto& it : c) {
        sum += it;
    }

    // Then
    Assertions::assertEquals(5050, sum);
};

auto should_to_array = []() {
    // Given
    util::ChainList<i32> c;
    c.append(1);
    c.append(2);
    c.append(3);
    c.append(4);
    c.append(5);

    // When
    auto arr = c.to_array();

    // Then
    Assertions::assertEquals(5, i32(arr.size()));
    Assertions::assertEquals("[1,2,3,4,5]"_cs, arr.__str__());
};

auto should_clear = []() {
    // Given
    util::ChainList<i32> c;
    c.append(1);
    c.append(2);
    c.append(3);
    c.append(4);
    c.append(5);

    // When
    c.clear();

    // Then
    Assertions::assertEquals(0, i32(c.size()));
    Assertions::assertTrue(c.empty());

    // When
    c.clear();

    // Then
    Assertions::assertTrue(c.empty());
};

auto should_str = []() {
    // Given
    util::ChainList<i32> c;
    c.append(1);
    c.append(2);
    c.append(3);
    c.append(4);
    c.append(5);

    // When
    CString s = c.__str__();

    // Then
    Assertions::assertEquals("<Chain> [1->2->3->4->5]"_cs, s);
};

auto test_chain() {
    UnitTestGroup group{"test_chain"};

    group.addTest("should_append", should_append);
    group.addTest("should_iterable", should_iterable);
    group.addTest("should_to_array", should_to_array);
    group.addTest("should_clear", should_clear);
    group.addTest("should_str", should_str);

    group.startAll();
}

} // namespace my::test::test_chain

#endif // TEST_CHAIN_HPP