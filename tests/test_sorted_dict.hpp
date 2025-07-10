#ifndef TEST_SORTED_DICT_HPP
#define TEST_SORTED_DICT_HPP

#include "Printer.hpp"
#include "UnitTest.hpp"
#include "Assertions.hpp"
#include "SortedDict.hpp"
#include <functional>

namespace my::test::test_sorted_dict {

fn it_works = []() {
    util::SortedDict<i32, i32> sd;
    Assertions::assertEquals("{}"_cs, sd.__str__());
};

fn should_insert = []() {
    // Given
    util::SortedDict<i32, i32> sd;

    // When
    sd.insert(17, 1);
    sd.insert(18, 2);
    sd.insert(23, 3);
    sd.insert(34, 4);
    sd.insert(27, 5);
    sd.insert(15, 6);
    sd.insert(9, 7);
    sd.insert(6, 8);
    sd.insert(8, 9);
    sd.insert(5, 10);
    sd.insert(25, 11);

    // Then
    io::println(sd.tree_struct());
    Assertions::assertEquals("{5:10,6:8,8:9,9:7,15:6,17:1,18:2,23:3,25:11,27:5,34:4}"_cs, sd.__str__());
};

fn should_insert_rev = []() {
    // Given
    util::SortedDict<i32, i32, std::greater<>> sd;

    // When
    sd.insert(17, 1);
    sd.insert(18, 2);
    sd.insert(23, 3);
    sd.insert(34, 4);
    sd.insert(27, 5);
    sd.insert(15, 6);
    sd.insert(9, 7);
    sd.insert(6, 8);
    sd.insert(8, 9);
    sd.insert(5, 10);
    sd.insert(25, 11);

    // Then
    io::println(sd.tree_struct());
    Assertions::assertEquals("{5:10,6:8,8:9,9:7,15:6,17:1,18:2,23:3,25:11,27:5,34:4}"_cs, sd.__str__());
};

fn should_construct_by_initializer_list = []() {
    // Given
    util::SortedDict<i32, i32> sd = {
        {17, 1},
        {18, 2},
        {23, 3},
        {34, 4},
        {27, 5},
        {15, 6},
        {9, 7},
        {6, 8},
        {8, 9},
        {5, 10},
        {25, 11},
    };
    
    // When & Then
    io::println(sd.tree_struct());
    Assertions::assertEquals("{5:10,6:8,8:9,9:7,15:6,17:1,18:2,23:3,25:11,27:5,34:4}"_cs, sd.__str__());
};

fn test_sorted_dict() {
    UnitTestGroup group{"test_sorted_dict"};

    group.addTest("it_works", it_works);
    group.addTest("should_insert", should_insert);
    group.addTest("should_insert_rev", should_insert_rev);

    group.startAll();
}

} // namespace my::test::test_sorted_dict

#endif // TEST_SORTED_DICT_HPP