#ifndef TEST_SPEED_HPP
#define TEST_SPEED_HPP

#include "test_dynarray.hpp"
#include "test_vector.hpp"
#include "test_dict.hpp"
#include "test_queue.hpp"
#include "test_string.hpp"
#include "test_code_point.hpp"
#include "test_thread_pool.hpp"

namespace my::test {

void test_speed() {
    test_dynarray::test_dynarray_speed();
    test_vec::test_vec_speed();
    test_dict::test_dict_speed();
    test_queue::test_queue_speed();
    test_string::test_string_builder_speed();
    test_code_point::test_code_point_pool_speed();
    test_thread_pool::test_thread_pool_speed();
}

} // namespace my::test

#endif // TEST_SPEED_HPP