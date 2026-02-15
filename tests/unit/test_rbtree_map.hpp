#ifndef TEST_RBTREE_MAP_HPP
#define TEST_RBTREE_MAP_HPP

#include "my_types.hpp"

namespace my::test::test_rbtree_map {
void it_works();
void it_works2();
void should_insert();
void should_insert_rev();
void should_construct_by_initializer_list();
void should_clone();
void should_for_each();
void should_get();
void should_fail_to_get_if_key_not_found();
void should_get_or_default();
void should_count();
void should_set_default();
void should_remove();
void should_iterable();
void should_operator();
void should_cmp();
void should_equals();
void test_rbtree_map();
void test_sorted_hash_map_operations_speed();
void test_map_operations_speed();
void test_rbtree_map_speed();
} // namespace my::test::test_rbtree_map

#endif // TEST_RBTREE_MAP_HPP
