#ifndef TEST_GRAPH_HPP
#define TEST_GRAPH_HPP

#include "ricky_test.hpp"
#include "Graph.hpp"

namespace my::test::test_graph {

fn should_create_graph = []() {
    // Given
    graph::Graph<char> g;

    // When
    g.add_vertex(1, 'a');
    g.add_vertex(2, 'b');
    g.add_vertex(3, 'c');
    g.add_vertex(4, 'd');
    g.add_vertex(5, 'e');
    g.add_vertex(6, 'f');
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
    Assertions::assertEquals(6, g.vertex_cnt());
    Assertions::assertEquals(10, g.edge_cnt());
    Assertions::assertEquals(2, g.edge_cnt(1));
    Assertions::assertEquals(-1, g.edge_cnt(7));
    io::print(g);
};

fn should_register = []() {
    // Given
    graph::Graph<char> g;
    g.add_vertex(1, 'a');
    g.add_vertex(2, 'b');
    g.add_vertex(3, 'c');
    g.add_vertex(4, 'd');
    g.add_vertex(5, 'e');
    g.add_vertex(6, 'f');
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
    g.register_algo("filter_edge", [](const auto& g, auto&& args) -> util::DynArray<i64> {
        auto min = opt<f64>(args, 0);
        auto max = opt<f64>(args, 1);

        util::DynArray<i64> res;
        for (const auto& edge : g.edges()) {
            if (math::fcmp(edge.w, min) > 0 && math::fcmp(edge.w, max) < 0) {
                res.append(edge.w);
            }
        }
        return res;
    });

    // When
    auto res = g.call_algo<util::DynArray<i64>>("filter_edge", 5.0, 25.0);

    // Then
    Assertions::assertEquals("[7,15,15,20]"_cs, res.__str__());
};

fn test_graph() {
    UnitTestGroup group{"test_graph"};

    group.addTest("should_create_graph", should_create_graph);
    group.addTest("should_register", should_register);

    group.startAll();
}

} // namespace my::test::test_graph

#endif // TEST_GRAPH_HPP