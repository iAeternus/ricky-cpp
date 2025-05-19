#ifndef TEST_DISJOINT_SET_HPP
#define TEST_DISJOINT_SET_HPP

#include "ricky_test.hpp"
#include "DisjointSet.hpp"

namespace my::test::test_disjoint_set {

auto should_merge_and_find = []() {
    util::DisjointSet<i32> ds = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    io::println(ds.__str__());

    ds.merge(5, 6);
    io::println(ds.__str__());

    ds.merge(1, 2);
    io::println(ds.__str__());

    ds.merge(2, 3);
    io::println(ds.__str__());

    ds.merge(1, 4);
    io::println(ds.__str__());

    ds.merge(1, 5);
    io::println(ds.__str__());

    Assertions::assertTrue(ds.is_connected(1, 5));
    Assertions::assertTrue(ds.is_connected(1, 6));
    Assertions::assertFalse(ds.is_connected(1, 8));

    for (i32 i = 1; i <= 10; ++i) {
        io::print(ds.rank(i));
    }
    io::println();
};

inline void test_disjoint_set() {
    UnitTestGroup group{"test_disjoint_set"};

    group.addTest("should_merge_and_find", should_merge_and_find);

    group.startAll();
}

} // namespace my::test::test_disjoint_set

#endif // TEST_DISJOINT_SET_HPP