// test_main.cpp
#include "test_test_utils.hpp"
#include "test_cstring.hpp"
#include "test_dynarray.hpp"
#include "test_chain.hpp"
#include "test_bi_chain.hpp"
#include "test_binary_utils.hpp"
#include "test_dist.hpp"
#include "test_random.hpp"
#include "test_string.hpp"
#include "test_array.hpp"
#include "test_thread_pool.hpp"
#include "test_win_file_utils.hpp"
#include "test_win_file.hpp"
#include "test_math_utils.hpp"
#include "test_big_integer.hpp"

#include "test_speed.hpp"

using namespace my::test;

/**
 * @brief 置1 测试功能，置0 测试性能，测试性能运行时间较长
 */
#define TEST_FUNCTIONALITY 1

int main() {

#if TEST_FUNCTIONALITY
    test_test_utils::should_group_unit_test();
    test_cstring::test_cstring();
    test_dynarray::test_dynarray();
    test_chain::test_chain();
    test_bi_chain::test_bi_chain();
    test_binary_utils::test_binary_utils();
    test_dict::test_dict();
    test_random::test_random();
    test_string::test_string();
    test_array::test_array();
    test_thread_pool::test_thread_pool();
    test_win_file_utils::test_win_file_utils();
    test_win_file::test_win_file();
    test_math_utils::test_math_utils();
    test_big_integer::test_big_integer();
#else
    test_speed();
#endif

}