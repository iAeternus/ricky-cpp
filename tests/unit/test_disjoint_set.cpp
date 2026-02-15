#include "test_disjoint_set.hpp"
#include "disjoint_set.hpp"
#include "ricky_test.hpp"

namespace my::test::test_disjoint_set {

void should_merge_and_find() {
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

    Assertions::assertTrue(ds.same_group(1, 5));
    Assertions::assertTrue(ds.same_group(1, 6));
    Assertions::assertFalse(ds.same_group(1, 8));

    for (usize i = 1; i <= 10; ++i) {
        io::print(ds.rank(i));
    }
    io::println();
}

GROUP_NAME("test_disjoint_set")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(should_merge_and_find))

} // namespace my::test::test_disjoint_set