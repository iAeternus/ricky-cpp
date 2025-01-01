#ifndef TEST_SPEED_HPP
#define TEST_SPEED_HPP

#include "test_dynarray.hpp"
#include "test_dist.hpp"
#include "test_thread_pool.hpp"

namespace my::test {

void test_speed() {
    test_dynarray::test_dynarray_speed();
    test_dict::test_dict_speed();
    test_thread_pool::test_thread_pool_speed();
}

} // namespace my::test

#endif // TEST_SPEED_HPP