#ifndef TEST_GRAPH_HPP
#define TEST_GRAPH_HPP

#include "my_types.hpp"

namespace my::test::test_graph {
void should_create_graph();
void should_fail_to_add_edge_if_node_not_found();
void should_register();
void should_fail_to_call_algo_if_algorithm_is_not_found();
void test_graph();
} // namespace my::test::test_graph

#endif // TEST_GRAPH_HPP