#include "test_speed.hpp"

#include "unit/test_dynarray.hpp"
#include "unit/test_vec.hpp"
#include "unit/test_hash_map.hpp"
#include "unit/test_queue.hpp"
#include "unit/test_string_builder.hpp"
#include "unit/test_code_point.hpp"
#include "unit/test_thread_pool.hpp"
#include "unit/test_rbtree_map.hpp"

#include "ricky_test.hpp"

namespace my::test {

void test_speed() {
    test_dynarray::test_dynarray_speed();
    test_vec::test_vec_speed();
    test_hash_map::test_hash_map_speed();
    test_queue::test_queue_speed();
    test_string_builder::test_string_builder_speed();
    test_thread_pool::test_thread_pool_speed();
    test_rbtree_map::test_rbtree_map_speed();
}

BENCH_NAME("test_speed")
REGISTER_BENCH_TESTS(
    BENCH_TEST_ITEM(test_speed))
} // namespace my::test