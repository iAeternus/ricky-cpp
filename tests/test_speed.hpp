#ifndef TEST_SPEED_HPP
#define TEST_SPEED_HPP

#include "test_dynarray.hpp"
#include "test_vec.hpp"
#include "test_hash_map.hpp"
#include "test_queue.hpp"
#include "test_string_builder.hpp"
#include "test_code_point.hpp"
#include "test_thread_pool.hpp"
#include "test_rbtree_map.hpp"

namespace my::test {

auto test_speed() {
    test_dynarray::test_dynarray_speed();
    test_vec::test_vec_speed();
    test_hash_map::test_hash_map_speed();
    test_queue::test_queue_speed();
    test_string_builder::test_string_builder_speed();
    test_thread_pool::test_thread_pool_speed();
    test_rbtree_map::test_rbtree_map_speed();
}

} // namespace my::test

#endif // TEST_SPEED_HPP