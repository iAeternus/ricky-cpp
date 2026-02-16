#include "test_graph.hpp"
#include "graph.hpp"
#include "printer.hpp"
#include "ricky_test.hpp"

namespace my::test::test_graph {

void should_create_graph() {
    // Given
    graph::Graph<char> g;

    // When
    g.add_node(1, 'a');
    g.add_node(2, 'b');
    g.add_node(3, 'c');
    g.add_node(4, 'd');
    g.add_node(5, 'e');
    g.add_node(6, 'f');
    g.add_edge(1, 2, 1);
    g.add_edge(1, 3, 7);
    g.add_edge(2, 1, 15);
    g.add_edge(2, 3, 2);
    g.add_edge(2, 4, 5);
    g.add_edge(3, 4, 1);
    g.add_edge(3, 5, -5);
    g.add_edge(4, 6, 4);
    g.add_edge(5, 4, 15);
    g.add_edge(5, 6, 20);

    // Then
    Assertions::assertTrue(g.is_directed());
    Assertions::assertEquals(6, g.node_cnt());
    Assertions::assertEquals(10, g.edge_cnt());
    Assertions::assertEquals(2, g.edge_cnt(1));
    Assertions::assertEquals(npos, g.edge_cnt(7));
    io::println(g);
}

void should_fail_to_add_edge_if_node_not_found() {
    // Given
    graph::Graph g;
    g.add_node(0);
    g.add_node(1);

    // When & Then
    Assertions::assertThrows("node from[1] or to[2] does not exist.", [&]() {
        g.add_edge(1, 2);
    });
}

void should_register() {
    // Given
    graph::Graph<char> g;
    g.add_node(1, 'a');
    g.add_node(2, 'b');
    g.add_node(3, 'c');
    g.add_node(4, 'd');
    g.add_node(5, 'e');
    g.add_node(6, 'f');
    g.add_edge(1, 2, 1);
    g.add_edge(1, 3, 7);
    g.add_edge(2, 1, 15);
    g.add_edge(2, 3, 2);
    g.add_edge(2, 4, 5);
    g.add_edge(3, 4, 1);
    g.add_edge(3, 5, -5);
    g.add_edge(4, 6, 4);
    g.add_edge(5, 4, 15);
    g.add_edge(5, 6, 20);

    // When
    // 注册自定义算法，使用新的插件系统
    g.register_algo("filter_edge", [](const auto& g, auto&& args) {
        using namespace my::graph;
        auto min = util::opt<f64>(args, 0);
        auto max = util::opt<f64>(args, 1);

        util::Vec<i64> res;
        for (const auto& edge : g.edges()) {
            if (math::fcmp(edge.w, min) > 0 && math::fcmp(edge.w, max) < 0) {
                res.push(edge.w);
            }
        }
        return res;
    });

    // When
    auto res = g.call_algo<util::Vec<i64>>("filter_edge", 5.0, 25.0);

    // Then
    Assertions::assertEquals("[7,15,15,20]"_cs, res.__str__());
}

void should_fail_to_call_algo_if_algorithm_is_not_found() {
    // Given
    graph::Graph g;

    // When & Then
    Assertions::assertThrows("algorithm[dij] not found.", [g]() {
        g.call_algo("dij", 1, 100);
    });
}

GROUP_NAME("test_graph")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(should_create_graph),
    UNIT_TEST_ITEM(should_fail_to_add_edge_if_node_not_found),
    UNIT_TEST_ITEM(should_register),
    UNIT_TEST_ITEM(should_fail_to_call_algo_if_algorithm_is_not_found))
} // namespace my::test::test_graph