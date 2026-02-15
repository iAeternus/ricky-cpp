#include "test_rbtree_map.hpp"
#include "printer.hpp"
#include "random.hpp"
#include "str.hpp"
#include "rbtree_map.hpp"
#include "ricky_test.hpp"

#include <map>

namespace my::test::test_rbtree_map {

void it_works() {
    util::RBTreeMap<i32, i32> t;
    util::Vec<i32> keys = {17, 18, 23, 34, 27, 15, 9, 6, 8, 5, 25};
    i32 idx = 1;

    for (const auto& key : keys) {
        t.insert(key, idx++);
        io::println(t.tree_struct());
        io::println("----------------------------------");
    }
}

void it_works2() {
    util::RBTreeMap<util::String, i32> t = {{"hello", 1}, {"world", 1}, {"你好", 2}, {"世界", 2}};
    Assertions::assertEquals("{hello:1,world:1,世界:2,你好:2}"_cs, t.__str__()); // TODO String前置声明，__str__
}

void should_insert() {
    // Given
    util::RBTreeMap<i32, i32> t;
    util::Vec<i32> keys = {17, 18, 23, 34, 27, 15, 9, 6, 8, 5, 25};
    i32 idx = 1;

    // Then
    Assertions::assertEquals("{}"_cs, t.__str__());
    Assertions::assertEquals(0, t.size());
    Assertions::assertTrue(t.empty());

    // When
    for (const auto& key : keys) {
        t.insert(key, idx++);
    }

    // Then
    Assertions::assertEquals("{5:10,6:8,8:9,9:7,15:6,17:1,18:2,23:3,25:11,27:5,34:4}"_cs, t.__str__());
    Assertions::assertEquals(11, t.size());
    Assertions::assertFalse(t.empty());
}

void should_insert_rev() {
    // Given
    util::RBTreeMap<i32, i32, std::greater<>> t;
    util::Vec<i32> keys = {17, 18, 23, 34, 27, 15, 9, 6, 8, 5, 25};
    i32 idx = 1;

    // When
    for (const auto& key : keys) {
        t.insert(key, idx++);
    }

    // Then
    Assertions::assertEquals("{34:4,27:5,25:11,23:3,18:2,17:1,15:6,9:7,8:9,6:8,5:10}"_cs, t.__str__());
    Assertions::assertEquals(11, t.size());
    Assertions::assertFalse(t.empty());
}

void should_construct_by_initializer_list() {
    // Given
    util::RBTreeMap<i32, i32> t = {{17, 1}, {18, 2}, {23, 3}, {34, 4}, {27, 5}, {15, 6}, {9, 7}, {6, 8}, {8, 9}, {5, 10}, {25, 11}};
    util::RBTreeMap<i32, i32, std::greater<>> t2 = {{17, 1}, {18, 2}, {23, 3}, {34, 4}, {27, 5}, {15, 6}, {9, 7}, {6, 8}, {8, 9}, {5, 10}, {25, 11}};

    // When & Then
    Assertions::assertEquals("{5:10,6:8,8:9,9:7,15:6,17:1,18:2,23:3,25:11,27:5,34:4}"_cs, t.__str__());
    Assertions::assertEquals(11, t.size());
    Assertions::assertFalse(t.empty());

    Assertions::assertEquals("{34:4,27:5,25:11,23:3,18:2,17:1,15:6,9:7,8:9,6:8,5:10}"_cs, t2.__str__());
    Assertions::assertEquals(11, t2.size());
    Assertions::assertFalse(t2.empty());
}

void should_clone() {
    // Given
    util::RBTreeMap<i32, i32> t = {{17, 1}, {18, 2}, {23, 3}, {34, 4}, {27, 5}, {15, 6}, {9, 7}, {6, 8}, {8, 9}, {5, 10}, {25, 11}};

    // When
    util::RBTreeMap<i32, i32> res(t);

    util::RBTreeMap<i32, i32> res2;
    res2 = t;

    // Then
    Assertions::assertEquals("{5:10,6:8,8:9,9:7,15:6,17:1,18:2,23:3,25:11,27:5,34:4}"_cs, t.__str__());
    Assertions::assertEquals("{5:10,6:8,8:9,9:7,15:6,17:1,18:2,23:3,25:11,27:5,34:4}"_cs, res.__str__());
    Assertions::assertEquals("{5:10,6:8,8:9,9:7,15:6,17:1,18:2,23:3,25:11,27:5,34:4}"_cs, res2.__str__());
}

void should_for_each() {
    // Given
    util::RBTreeMap<i32, i32> t = {{17, 1}, {18, 2}, {23, 3}, {34, 4}, {27, 5}, {15, 6}, {9, 7}, {6, 8}, {8, 9}, {5, 10}, {25, 11}};
    util::Vec<i32> res, res2;

    // When
    t.for_each([&](const auto& k, const auto& v) {
        res.push(k);
    });

    t.for_each_rev([&](const auto& k, const auto& v) {
        res2.push(k);
    });

    // Then
    Assertions::assertEquals("[5,6,8,9,15,17,18,23,25,27,34]"_cs, res.__str__());
    Assertions::assertEquals("[34,27,25,23,18,17,15,9,8,6,5]"_cs, res2.__str__());
}

void should_get() {
    // Given
    util::RBTreeMap<i32, i32> t = {{17, 1}, {18, 2}, {23, 3}, {34, 4}, {27, 5}, {15, 6}, {9, 7}, {6, 8}, {8, 9}, {5, 10}, {25, 11}};

    // When
    auto res = t.get(15);
    auto res2 = t.get(6);
    auto res3 = t.get(34);

    // Then
    Assertions::assertEquals(6, res);
    Assertions::assertEquals(8, res2);
    Assertions::assertEquals(4, res3);
}

void should_fail_to_get_if_key_not_found() {
    // Given
    util::RBTreeMap<i32, i32> t = {{17, 1}, {18, 2}, {23, 3}, {34, 4}, {27, 5}, {15, 6}, {9, 7}, {6, 8}, {8, 9}, {5, 10}, {25, 11}};

    // When & Then
    Assertions::assertThrows("key '99' not found in red-black-tree", [&]() {
        t.get(99);
    });

    Assertions::assertThrows("key '0' not found in red-black-tree", [&]() {
        t.get(0);
    });
}

void should_get_or_default() {
    // Given
    util::RBTreeMap<i32, i32> t = {{17, 1}, {18, 2}, {23, 3}, {34, 4}, {27, 5}, {15, 6}, {9, 7}, {6, 8}, {8, 9}, {5, 10}, {25, 11}};

    // When
    auto res = t.get_or_default(15, 10);
    auto res2 = t.get_or_default(99, 10);

    // Then
    Assertions::assertEquals(6, res);
    Assertions::assertEquals(10, res2);
}

void should_count() {
    // Given
    util::RBTreeMap<i32, i32> t;
    util::Vec<i32> v = {1, 1, 1, 2, 2, 3, 4, 4, 4, 4};

    // When
    for (const auto& it : v) {
        t[it]++;
    }

    // Then
    Assertions::assertEquals("{1:3,2:2,3:1,4:4}"_cs, t.__str__());
}

void should_set_default() {
    // Given
    util::RBTreeMap<i32, i32> t = {{17, 1}, {18, 2}, {23, 3}, {34, 4}, {27, 5}, {15, 6}, {9, 7}, {6, 8}, {8, 9}, {5, 10}, {25, 11}};

    // When
    t.set_default(17, 0).set_default(99, 0);

    // Then
    Assertions::assertEquals(1, t[17]);
    Assertions::assertEquals(0, t[99]);
}

void should_remove() {
    // Given
    util::RBTreeMap<i32, i32> t = {{15, 1}, {9, 2}, {18, 3}, {6, 4}, {13, 5}, {17, 6}, {27, 7}, {10, 8}, {23, 9}, {34, 10}, {25, 11}, {37, 12}};
    util::Vec<i32> keys = {18, 25, 15, 6, 13, 37, 27, 17, 34, 9, 10, 23};

    // When
    for (const auto& key : keys) {
        io::println(t.tree_struct());
        io::println("----------------------------------");
        t.remove(key);
    }

    // Then
    Assertions::assertEquals(0, t.size());
    Assertions::assertTrue(t.empty());
}

void should_iterable() {
    // Given
    util::RBTreeMap<i32, i32> t = {{17, 1}, {18, 2}, {23, 3}, {34, 4}, {27, 5}, {15, 6}, {9, 7}, {6, 8}, {8, 9}, {5, 10}, {25, 11}};
    util::Vec<i32> keys, values;

    // When
    for (const auto& [k, v] : t) {
        keys.push(k);
        values.push(v);
    }

    // Then
    Assertions::assertEquals("[5,6,8,9,15,17,18,23,25,27,34]"_cs, keys.__str__());
    Assertions::assertEquals("[10,8,9,7,6,1,2,3,11,5,4]"_cs, values.__str__());
}

void should_operator() {
    // Given
    util::RBTreeMap<i32, i32> t = {{1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}};
    util::RBTreeMap<i32, i32> t2 = {{4, 1}, {5, 1}, {6, 1}, {7, 1}, {8, 1}};

    // When
    auto res = t & t2;  // 交集
    auto res2 = t | t2; // 并集
    auto res3 = t ^ t2; // 相对补集
    auto res4 = t - t2; // 差集

    // Then
    Assertions::assertEquals(2, i32(res.size()));
    Assertions::assertEquals(8, i32(res2.size()));
    Assertions::assertEquals(6, i32(res3.size()));
    Assertions::assertEquals(3, i32(res4.size()));
}

void should_cmp() {
    // Given
    util::RBTreeMap<i32, i32> t = {{1, 1}, {2, 2}, {3, 3}};
    util::RBTreeMap<i32, i32> t2 = {{1, 1}, {2, 2}, {3, 3}};
    util::RBTreeMap<i32, i32> t3 = {{1, 1}, {3, 3}};
    util::RBTreeMap<i32, i32> t4 = {{2, 2}, {4, 4}};

    // When
    auto res = t.__cmp__(t2);
    auto res2 = t.__cmp__(t3);
    auto res3 = t3.__cmp__(t);
    auto res4 = t3.__cmp__(t4);

    // Then
    Assertions::assertEquals(0, res);
    Assertions::assertEquals(1, res2);
    Assertions::assertEquals(-1, res3);
    Assertions::assertEquals(TYPE_MAX(cmp_t), res4);
}

void should_equals() {
    // Given
    util::RBTreeMap<i32, i32> t = {{17, 1}, {18, 2}, {23, 3}, {34, 4}, {27, 5}, {15, 6}, {9, 7}, {6, 8}, {8, 9}, {5, 10}, {25, 11}};
    util::RBTreeMap<i32, i32> t2 = {{17, 1}, {18, 2}, {23, 3}, {34, 4}, {27, 5}, {15, 6}, {9, 7}, {6, 8}, {8, 9}, {5, 10}, {25, 11}};
    util::RBTreeMap<i32, i32> t3 = {{1, 1}};

    // When
    auto res = t.__equals__(t2);
    auto res2 = t.__equals__(t3);

    // Then
    Assertions::assertTrue(res);
    Assertions::assertFalse(res2);
}

GROUP_NAME("test_rbtree_map")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(it_works),
    UNIT_TEST_ITEM(it_works2),
    UNIT_TEST_ITEM(should_insert),
    UNIT_TEST_ITEM(should_insert_rev),
    UNIT_TEST_ITEM(should_construct_by_initializer_list),
    UNIT_TEST_ITEM(should_clone),
    UNIT_TEST_ITEM(should_for_each),
    UNIT_TEST_ITEM(should_get),
    UNIT_TEST_ITEM(should_fail_to_get_if_key_not_found),
    UNIT_TEST_ITEM(should_get_or_default),
    UNIT_TEST_ITEM(should_count),
    UNIT_TEST_ITEM(should_set_default),
    UNIT_TEST_ITEM(should_remove),
    UNIT_TEST_ITEM(should_iterable),
    UNIT_TEST_ITEM(should_operator),
    UNIT_TEST_ITEM(should_cmp),
    UNIT_TEST_ITEM(should_equals))

} // namespace my::test::test_rbtree_map
