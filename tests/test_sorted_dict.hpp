#ifndef TEST_SORTED_DICT_HPP
#define TEST_SORTED_DICT_HPP

#include "UnitTest.hpp"
#include "Assertions.hpp"
#include "Printer.hpp"
#include "Random.hpp"
#include "String.hpp"
#include "SortedDict.hpp"
#include "ricky.hpp"

#include <map>

namespace my::test::test_sorted_dict {

fn it_works = []() {
    util::SortedDict<i32, i32> sd;
    util::Vec<i32> keys = {17, 18, 23, 34, 27, 15, 9, 6, 8, 5, 25};
    i32 idx = 1;

    for (const auto& key : keys) {
        sd.insert(key, idx++);
        io::println(sd.tree_struct());
        io::println("----------------------------------");
    }
};

fn it_works2 = []() {
    util::SortedDict<util::String, i32> sd = {{"hello"_s, 1}, {"world"_s, 1}, {"你好"_s, 2}, {"世界", 2}};
    Assertions::assertEquals("{\"hello\":1,\"world\":1,\"世界\":2,\"你好\":2}"_cs, sd.__str__());
};

fn should_insert = []() {
    // Given
    util::SortedDict<i32, i32> sd;
    util::Vec<i32> keys = {17, 18, 23, 34, 27, 15, 9, 6, 8, 5, 25};
    i32 idx = 1;

    // Then
    Assertions::assertEquals("{}"_cs, sd.__str__());
    Assertions::assertEquals(0, sd.size());
    Assertions::assertTrue(sd.empty());

    // When
    for (const auto& key : keys) {
        sd.insert(key, idx++);
    }

    // Then
    Assertions::assertEquals("{5:10,6:8,8:9,9:7,15:6,17:1,18:2,23:3,25:11,27:5,34:4}"_cs, sd.__str__());
    Assertions::assertEquals(11, sd.size());
    Assertions::assertFalse(sd.empty());
};

fn should_insert_rev = []() {
    // Given
    util::SortedDict<i32, i32, std::greater<>> sd;
    util::Vec<i32> keys = {17, 18, 23, 34, 27, 15, 9, 6, 8, 5, 25};
    i32 idx = 1;

    // When
    for (const auto& key : keys) {
        sd.insert(key, idx++);
    }

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

fn should_get = []() {
    // Given
    util::SortedDict<i32, i32> sd = {{17, 1}, {18, 2}, {23, 3}, {34, 4}, {27, 5}, {15, 6}, {9, 7}, {6, 8}, {8, 9}, {5, 10}, {25, 11}};

    // When
    auto res = sd.get(15);
    auto res2 = sd.get(6);
    auto res3 = sd.get(34);

    // Then
    Assertions::assertEquals(6, res);
    Assertions::assertEquals(8, res2);
    Assertions::assertEquals(4, res3);
};

fn should_fail_to_get_if_key_not_found = []() {
    // Given
    util::SortedDict<i32, i32> sd = {{17, 1}, {18, 2}, {23, 3}, {34, 4}, {27, 5}, {15, 6}, {9, 7}, {6, 8}, {8, 9}, {5, 10}, {25, 11}};

    // When & Then
    Assertions::assertThrows("key '99' not found in red-black-tree", [&]() {
        sd.get(99);
    });

    Assertions::assertThrows("key '0' not found in red-black-tree", [&]() {
        sd.get(0);
    });
};

fn should_get_or_default = []() {
    // Given
    util::SortedDict<i32, i32> sd = {{17, 1}, {18, 2}, {23, 3}, {34, 4}, {27, 5}, {15, 6}, {9, 7}, {6, 8}, {8, 9}, {5, 10}, {25, 11}};

    // When
    auto res = sd.get_or_default(15, 10);
    auto res2 = sd.get_or_default(99, 10);

    // Then
    Assertions::assertEquals(6, res);
    Assertions::assertEquals(10, res2);
};

fn should_count = []() {
    // Given
    util::SortedDict<i32, i32> sd;
    util::Vec<i32> v = {1, 1, 1, 2, 2, 3, 4, 4, 4, 4};

    // When
    for (const auto& it : v) {
        sd[it]++;
    }

    // Then
    Assertions::assertEquals("{1:3,2:2,3:1,4:4}"_cs, sd.__str__());
};

fn should_set_default = []() {
    // Given
    util::SortedDict<i32, i32> sd = {{17, 1}, {18, 2}, {23, 3}, {34, 4}, {27, 5}, {15, 6}, {9, 7}, {6, 8}, {8, 9}, {5, 10}, {25, 11}};

    // When
    sd.set_default(17, 0).set_default(99, 0);

    // Then
    Assertions::assertEquals(1, sd[17]);
    Assertions::assertEquals(0, sd[99]);
};

fn should_remove = []() {
    // Given
    util::SortedDict<i32, i32> sd = {{15, 1}, {9, 2}, {18, 3}, {6, 4}, {13, 5}, {17, 6}, {27, 7}, {10, 8}, {23, 9}, {34, 10}, {25, 11}, {37, 12}};
    util::Vec<i32> keys = {18, 25, 15, 6, 13, 37, 27, 17, 34, 9, 10, 23};

    // When
    for (const auto& key : keys) {
        io::println(sd.tree_struct());
        io::println("----------------------------------");
        sd.remove(key);
    }

    // Then
    Assertions::assertEquals(0, sd.size());
    Assertions::assertTrue(sd.empty());
};

fn should_iterable = []() {
    // Given
    util::SortedDict<i32, i32> sd = {{17, 1}, {18, 2}, {23, 3}, {34, 4}, {27, 5}, {15, 6}, {9, 7}, {6, 8}, {8, 9}, {5, 10}, {25, 11}};
    util::Vec<i32> keys, values;

    // When
    for (const auto& [k, v] : sd) {
        keys.append(k);
        values.append(v);
    }

    // Then
    Assertions::assertEquals("[5,6,8,9,15,17,18,23,25,27,34]"_cs, keys.__str__());
    Assertions::assertEquals("[10,8,9,7,6,1,2,3,11,5,4]"_cs, values.__str__());
};

fn should_operator = []() {
    // Given
    util::SortedDict<i32, i32> sd = {{1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}};
    util::SortedDict<i32, i32> sd2 = {{4, 1}, {5, 1}, {6, 1}, {7, 1}, {8, 1}};

    // When
    auto res = sd & sd2;  // 交集
    auto res2 = sd | sd2; // 并集
    auto res3 = sd ^ sd2; // 相对补集
    auto res4 = sd - sd2; // 差集

    // Then
    Assertions::assertEquals(2, i32(res.size()));
    Assertions::assertEquals(8, i32(res2.size()));
    Assertions::assertEquals(6, i32(res3.size()));
    Assertions::assertEquals(3, i32(res4.size()));
};

fn should_cmp = []() {
    // Given
    util::SortedDict<i32, i32> sd = {{1, 1}, {2, 2}, {3, 3}};
    util::SortedDict<i32, i32> sd2 = {{1, 1}, {2, 2}, {3, 3}};
    util::SortedDict<i32, i32> sd3 = {{1, 1}, {3, 3}};
    util::SortedDict<i32, i32> sd4 = {{2, 2}, {4, 4}};

    // When
    auto res = sd.__cmp__(sd2);
    auto res2 = sd.__cmp__(sd3);
    auto res3 = sd3.__cmp__(sd);
    auto res4 = sd3.__cmp__(sd4);

    // Then
    Assertions::assertEquals(0, res);
    Assertions::assertEquals(1, res2);
    Assertions::assertEquals(-1, res3);
    Assertions::assertEquals(TYPE_MAX(cmp_t), res4);
};

fn should_equals = []() {
    // Given
    util::SortedDict<i32, i32> sd = {{17, 1}, {18, 2}, {23, 3}, {34, 4}, {27, 5}, {15, 6}, {9, 7}, {6, 8}, {8, 9}, {5, 10}, {25, 11}};
    util::SortedDict<i32, i32> sd2 = {{17, 1}, {18, 2}, {23, 3}, {34, 4}, {27, 5}, {15, 6}, {9, 7}, {6, 8}, {8, 9}, {5, 10}, {25, 11}};
    util::SortedDict<i32, i32> sd3 = {{1, 1}};

    // When
    auto res = sd.__equals__(sd2);
    auto res2 = sd.__equals__(sd3);

    // Then
    Assertions::assertTrue(res);
    Assertions::assertFalse(res2);
};

fn test_sorted_dict() {
    UnitTestGroup group{"test_sorted_dict"};

    group.addTest("it_works", it_works);
    group.addTest("it_works2", it_works2);
    group.addTest("should_insert", should_insert);
    group.addTest("should_insert_rev", should_insert_rev);
    group.addTest("should_construct_by_initializer_list", should_construct_by_initializer_list);
    group.addTest("should_clone", should_clone);
    group.addTest("should_for_each", should_for_each);
    group.addTest("should_get", should_get);
    group.addTest("should_fail_to_get_if_key_not_found", should_fail_to_get_if_key_not_found);
    group.addTest("should_get_or_default", should_get_or_default);
    group.addTest("should_count", should_count);
    group.addTest("should_set_default", should_set_default);
    group.addTest("should_remove", should_remove);
    group.addTest("should_iterable", should_iterable);
    group.addTest("should_operator", should_operator);
    group.addTest("should_cmp", should_cmp);
    group.addTest("should_equals", should_equals);

    group.startAll();
}

fn test_sorted_dict_speed() {
    i32 n = 1e6;
    util::Vec<i32> nums;
    UnitTestGroup group{"test_sorted_dict_speed"};

    group.setup([&]() {
        for (i32 i = 0; i < n; ++i) {
            nums.append(util::Random::instance().next<i32>(1, n));
        }
    });

    group.addTest("test_sorted_dict_operations_speed", [&]() {
        util::SortedDict<i32, i32> sd;

        // insert
        for (i32 i = 0; i < n; ++i) {
            sd.insert(nums[i], 0);
        }

        // get
        for (i32 i = 0; i < n; ++i) {
            sd[nums[i]]++;
        }

        // remove
        for (i32 i = 0; i < n; ++i) {
            sd.remove(nums[i]);
        }
    });

    group.addTest("test_map_operations_speed", [&]() {
        std::map<i32, i32> mp;

        // insert
        for (i32 i = 0; i < n; ++i) {
            mp.emplace(nums[i], 0);
        }

        // get
        for (i32 i = 0; i < n; ++i) {
            mp[nums[i]]++;
        }

        // remove
        for (i32 i = 0; i < n; ++i) {
            mp.erase(nums[i]);
        }
    });

    group.startAll();
}

} // namespace my::test::test_sorted_dict

#endif // TEST_SORTED_DICT_HPP