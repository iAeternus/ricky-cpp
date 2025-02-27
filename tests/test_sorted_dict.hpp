#ifndef TEST_SORTED_DICT_HPP
#define TEST_SORTED_DICT_HPP

#include "ricky_test.hpp"
#include "SortedDict.hpp"

namespace my::test::test_sorted_dict {

auto it_works = []() {
    util::SortedDict<i32, i32> sd;
    Assertions::assertEquals("(empty dict)"_cs, sd.__str__());
};

void test_sorted_dict() {
    UnitTestGroup group{"test_sorted_dict"};

    group.addTest("it_works", it_works);

    group.startAll();
}

} // namespace my::test::test_sorted_dict

#endif // TEST_SORTED_DICT_HPP