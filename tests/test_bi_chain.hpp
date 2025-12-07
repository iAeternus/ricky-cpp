#ifndef TEST_BI_CHAIN_HPP
#define TEST_BI_CHAIN_HPP

#include "ricky_test.hpp"
#include "bi_chain.hpp"

namespace my::test::test_bi_chain {

auto should_append_and_prepend = []() {
    // Given
    util::BiChainList<CString> bc;

    // When
    bc.append(cstr(3));
    bc.append(cstr(4));
    bc.append(cstr(5));
    bc.prepend(cstr(2));
    bc.prepend(cstr(1));

    // Then
    Assertions::assertEquals(5, i32(bc.size()));
    Assertions::assertEquals("<BiChain> [1<-->2<-->3<-->4<-->5]"_cs, bc.__str__());
};

auto should_iterable = []() {
    // Given
    i32 N = 100;
    util::BiChainList<i32> bc;
    for (usize i = 0; i < N; ++i) {
        bc.append(i + 1);
    }

    // When
    i32 sum1 = 0;
    for (const auto& it : bc) {
        sum1 += it;
    }

    // Then
    Assertions::assertEquals(5050, sum1);

    // // When
    // // TODO 这里有问题，段错误，不能反向迭代
    // i32 sum2 = 0;
    // for (auto it = bc.rbegin(); it != bc.rend(); ++it) {
    //     sum2 += *it;
    // }

    // // Then
    // Assertions::assertEquals(sum1, sum2);
};

auto test_bi_chain() {
    UnitTestGroup group{"test_bi_chain"};

    group.addTest("should_append_and_prepend", should_append_and_prepend);
    group.addTest("should_iterable", should_iterable);

    group.startAll();
}

} // namespace my::test::test_bi_chain

#endif // TEST_BI_CHAIN_HPP