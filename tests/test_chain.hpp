#ifndef TEST_CHAIN_HPP
#define TEST_CHAIN_HPP

#include "ricky_test.hpp"
#include "Chain.hpp"

using namespace my;

namespace my::test::test_chain {

auto should_append = []() {
    // Given
    int N = 100;
    util::Chain<util::ChainNode<CString>> c;

    // When
    for(int i = 0; i < N; ++i) {
        c.append(cstr(i));
    }

    // Then
    test::Assertions::assertEquals(N, int(c.size()));
    test::Assertions::assertFalse(c.empty());
    test::Assertions::assertTrue(c.contains(cstr(N - 1)));
    test::Assertions::assertFalse(c.contains(cstr(N)));
    test::Assertions::assertEquals(cstr(N - 1), c[N - 1]);
};

auto should_iterable = []() {
    // Given
    int N = 100;
    util::Chain<util::ChainNode<int>> c;
    for(int i = 0; i < N; ++i) {
        c.append(i + 1);
    }

    // When
    int sum = 0;
    for(const auto& it : c) {
        sum += it;
    }

    // Then
    test::Assertions::assertEquals(5050, sum);
};

auto should_to_array = []() {
    // Given
    util::Chain<util::ChainNode<int>> c;
    c.append(1);
    c.append(2);
    c.append(3);
    c.append(4);
    c.append(5);

    // When
    auto arr = c.toArray();

    // Then
    test::Assertions::assertEquals(5, int(arr.size()));
    test::Assertions::assertEquals(CString{"[1,2,3,4,5]"}, arr.__str__());
};

auto should_clear = []() {
    // Given
    util::Chain<util::ChainNode<int>> c;
    c.append(1);
    c.append(2);
    c.append(3);
    c.append(4);
    c.append(5);

    // When
    c.clear();

    // Then
    test::Assertions::assertEquals(0, int(c.size()));
    test::Assertions::assertTrue(c.empty());
    test::Assertions::assertEquals(c.begin(), c.end());

    // When
    c.clear();

    // Then
    test::Assertions::assertTrue(c.empty());
};

void test_chain() {
    test::UnitTestGroup group("test_chain");

    group.addTest("should_append", should_append);
    group.addTest("should_iterable", should_iterable);
    group.addTest("should_to_array", should_to_array);
    group.addTest("should_clear", should_clear);

    group.startAll();
}

} // namespace my::test::test_chain

#endif // TEST_CHAIN_HPP