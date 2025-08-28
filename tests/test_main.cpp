/**
 * @brief 测试启动器
 * @author Ricky
 * @date 2024/12/1
 * @version 2.0 优化了测试模式宏定义
 * @file test_main.cpp
 */

/**
 * 测试模式设置
 * - 0 基准测试（运行时间较长）
 * - 1 功能测试
 */
#define TEST_MODE 0

#if TEST_MODE == 0
#include "test_speed.hpp"
#elif TEST_MODE == 1
#include "test_test_utils.hpp"
#include "test_cstring.hpp"
#include "test_buffer.hpp"
#include "test_dynarray.hpp"
#include "test_chain.hpp"
#include "test_bi_chain.hpp"
#include "test_binary_utils.hpp"
#include "test_dict.hpp"
#include "test_random.hpp"
#include "test_code_point.hpp"
#include "test_string.hpp"
#include "test_string_builder.hpp"
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
#include "test_graph_algorithm.hpp"
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
#include "test_sorted_dict.hpp"
#include "test_vec.hpp"
#include "test_expr.hpp"
#include "test_vector2.hpp"
#include "test_geometry_2d.hpp"
#include "test_polygon.hpp"
#include "test_huffman_tree.hpp"
#include "test_big_decimal.hpp"
#include "test_priority_queue.hpp"
#include "test_allocator.hpp"
#include "test_match.hpp"
#include "test_udp.hpp"
#include "test_tcp.hpp"
#include "test_json_serializer.hpp"
#include "test_string_algorithm.hpp"
#endif

#include <winnls.h>

using namespace my::test;

auto main() -> int {
    system(("chcp " + std::to_string(CP_UTF8)).c_str()); // 控制台输出ASC颜色字符

#if TEST_MODE == 0
    test_speed();
#elif TEST_MODE == 1
    test_test_utils::test_test_utils();
    test_cstring::test_cstring();
    test_buffer::test_buffer();
    test_dynarray::test_dynarray();
    test_chain::test_chain();
    test_bi_chain::test_bi_chain();
    test_binary_utils::test_binary_utils();
    test_dict::test_dict();
    test_random::test_random();
    test_code_point::test_code_point();
    test_string::test_string();
    test_string_builder::test_string_builder();
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
    test_graph_algorithm::test_graph_algorithm();
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
    test_sorted_dict::test_sorted_dict();
    test_vec::test_vec();
    test_expr::test_expr();
    test_vector2::test_vector2();
    test_geometry_2d::test_geometry_2d();
    test_polygon::test_polygon();
    test_huffman_tree::test_huffman_tree();
    test_big_decimal::test_big_decimal();
    test_priority_queue::test_priority_queue();
    test_allocator::test_allocator();
    test_match::test_match();
    test_udp::test_udp();
    test_tcp::test_tcp();
    test_json_serializer::test_json_serializer();
    test_string_algorithm::test_string_algorithm();
#endif
}