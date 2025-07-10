#ifndef TEST_SORTED_DICT_HPP
#define TEST_SORTED_DICT_HPP

#include "Printer.hpp"
#include "UnitTest.hpp"
#include "Assertions.hpp"
#include "SortedDict.hpp"
#include "Vec.hpp"
#include <functional>

namespace my::test::test_sorted_dict {

fn it_works = []() {
    util::SortedDict<i32, i32> sd;

    sd.insert(17, 1);
    io::println(sd.tree_struct());
    io::println("----------------------------------");
    sd.insert(18, 2);
    io::println(sd.tree_struct());
    io::println("----------------------------------");
    sd.insert(23, 3);
    io::println(sd.tree_struct());
    io::println("----------------------------------");
    sd.insert(34, 4);
    io::println(sd.tree_struct());
    io::println("----------------------------------");
    sd.insert(27, 5);
    io::println(sd.tree_struct());
    io::println("----------------------------------");
    sd.insert(15, 6);
    io::println(sd.tree_struct());
    io::println("----------------------------------");
    sd.insert(9, 7);
    io::println(sd.tree_struct());
    io::println("----------------------------------");
    sd.insert(6, 8);
    io::println(sd.tree_struct());
    io::println("----------------------------------");
    sd.insert(8, 9);
    io::println(sd.tree_struct());
    io::println("----------------------------------");
    sd.insert(5, 10);
    io::println(sd.tree_struct());
    io::println("----------------------------------");
    sd.insert(25, 11);
    io::println(sd.tree_struct());
};

fn should_insert = []() {
    // Given
    util::SortedDict<i32, i32> sd;

    // Then
    Assertions::assertEquals("{}"_cs, sd.__str__());
    Assertions::assertEquals(0, sd.size());
    Assertions::assertTrue(sd.empty());

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
    Assertions::assertEquals("{5:10,6:8,8:9,9:7,15:6,17:1,18:2,23:3,25:11,27:5,34:4}"_cs, sd.__str__());
    Assertions::assertEquals(11, sd.size());
    Assertions::assertFalse(sd.empty());
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
    Assertions::assertEquals("{34:4,27:5,25:11,23:3,18:2,17:1,15:6,9:7,8:9,6:8,5:10}"_cs, sd.__str__());
    Assertions::assertEquals(11, sd.size());
    Assertions::assertFalse(sd.empty());
};

fn should_construct_by_initializer_list = []() {
    // Given
    util::SortedDict<i32, i32> sd = {{17, 1}, {18, 2}, {23, 3}, {34, 4}, {27, 5}, {15, 6}, {9, 7}, {6, 8}, {8, 9}, {5, 10}, {25, 11}};
    util::SortedDict<i32, i32, std::greater<>> sd2 = {{17, 1}, {18, 2}, {23, 3}, {34, 4}, {27, 5}, {15, 6}, {9, 7}, {6, 8}, {8, 9}, {5, 10}, {25, 11}};

    // When & Then
    Assertions::assertEquals("{5:10,6:8,8:9,9:7,15:6,17:1,18:2,23:3,25:11,27:5,34:4}"_cs, sd.__str__());
    Assertions::assertEquals(11, sd.size());
    Assertions::assertFalse(sd.empty());

    Assertions::assertEquals("{34:4,27:5,25:11,23:3,18:2,17:1,15:6,9:7,8:9,6:8,5:10}"_cs, sd2.__str__());
    Assertions::assertEquals(11, sd2.size());
    Assertions::assertFalse(sd2.empty());
};

fn should_clone = []() {
    // Given
    util::SortedDict<i32, i32> sd = {{17, 1}, {18, 2}, {23, 3}, {34, 4}, {27, 5}, {15, 6}, {9, 7}, {6, 8}, {8, 9}, {5, 10}, {25, 11}};

    // When
    util::SortedDict<i32, i32> res(sd);

    util::SortedDict<i32, i32> res2;
    res2 = sd;

    // Then
    Assertions::assertEquals("{5:10,6:8,8:9,9:7,15:6,17:1,18:2,23:3,25:11,27:5,34:4}"_cs, sd.__str__());
    Assertions::assertEquals("{5:10,6:8,8:9,9:7,15:6,17:1,18:2,23:3,25:11,27:5,34:4}"_cs, res.__str__());
    Assertions::assertEquals("{5:10,6:8,8:9,9:7,15:6,17:1,18:2,23:3,25:11,27:5,34:4}"_cs, res2.__str__());
};

fn should_for_each = []() {
    // Given
    util::SortedDict<i32, i32> sd = {{17, 1}, {18, 2}, {23, 3}, {34, 4}, {27, 5}, {15, 6}, {9, 7}, {6, 8}, {8, 9}, {5, 10}, {25, 11}};
    util::Vec<i32> res, res2;

    // When
    sd.for_each([&](const auto& k, const auto& v) {
        res.append(k);
    });

    sd.for_each_rev([&](const auto& k, const auto& v) {
        res2.append(k);
    });

    // Then
    Assertions::assertEquals("[5,6,8,9,15,17,18,23,25,27,34]"_cs, res.__str__());
    Assertions::assertEquals("[34,27,25,23,18,17,15,9,8,6,5]"_cs, res2.__str__());
};

fn test_sorted_dict() {
    UnitTestGroup group{"test_sorted_dict"};

    group.addTest("it_works", it_works);
    group.addTest("should_insert", should_insert);
    group.addTest("should_insert_rev", should_insert_rev);
    group.addTest("should_construct_by_initializer_list", should_construct_by_initializer_list);
    group.addTest("should_clone", should_clone);
    group.addTest("should_for_each", should_for_each);

    group.startAll();
}

} // namespace my::test::test_sorted_dict

#endif // TEST_SORTED_DICT_HPP