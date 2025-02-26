/**
 * @brief 测试启动器
 * @author Ricky
 * @date 2024/12/1
 * @version 2.0 优化了测试模式宏定义
 * @file test_main.cpp
 */
#include "test_test_utils.hpp"
#include "test_cstring.hpp"
#include "test_buffer.hpp"
#include "test_dynarray.hpp"
#include "test_chain.hpp"
#include "test_bi_chain.hpp"
#include "test_binary_utils.hpp"
#include "test_dict.hpp"
#include "test_random.hpp"
#include "test_string.hpp"
#include "test_array.hpp"
#include "test_thread_pool.hpp"
#include "test_win_file_utils.hpp"
#include "test_win_file.hpp"
#include "test_math_utils.hpp"
#include "test_big_integer.hpp"
#include "test_pair.hpp"
#include "test_json_parser.hpp"
#include "test_complex.hpp"
#include "test_matrix.hpp"
#include "test_disjoint_set.hpp"
#include "test_graph.hpp"
#include "test_generator.hpp"
#include "test_stream.hpp"
#include "test_duration.hpp"
#include "test_date.hpp"
#include "test_time.hpp"
#include "test_date_time.hpp"
#include "test_stack.hpp"
#include "test_queue.hpp"
#include "test_log.hpp"
#include "test_tree.hpp"

#include "test_speed.hpp"

using namespace my::test;

/**
 * @brief 测试模式设置
 * @note 置1 功能测试；置0 基准测试（运行时间较长）
 */
#define TEST_MODE 1

int main() {
#if TEST_MODE
    test_test_utils::test_test_utils();
    test_cstring::test_cstring();
    test_buffer::test_buffer();
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
    test_pair::test_pair();
    test_json_parser::test_json_parser();
    test_complex::test_complex();
    test_matrix::test_matrix();
    test_disjoint_set::test_disjoint_set();
    test_graph::test_graph();
    test_generator::test_generator();
    test_stream::test_stream();
    test_duration::test_duration();
    test_date::test_date();
    test_time::test_time();
    test_date_time::test_date_time();
    test_stack::test_stack();
    test_queue::test_queue();
    test_log::test_log();
    test_tree::test_tree();
#else
    test_speed();
#endif
}