#ifndef TEST_GRAPH_HPP
#define TEST_GRAPH_HPP

#include "ricky_test.hpp"
#include "Graph.hpp"

namespace my::test::test_graph {

auto should_create_graph = []() {
    // Given
    graph::Graph<char> g;

    // When
    g.addVertex(1, 'a');
    g.addVertex(2, 'b');
    g.addVertex(3, 'c');
    g.addVertex(4, 'd');
    g.addVertex(5, 'e');
    g.addVertex(6, 'f');
    g.addEdge(1, 2, 1);
    g.addEdge(1, 3, 7);
    g.addEdge(2, 1, 15);
    g.addEdge(2, 3, 2);
    g.addEdge(2, 4, 5);
    g.addEdge(3, 4, 1);
    g.addEdge(3, 5, -5);
    g.addEdge(4, 6, 4);
    g.addEdge(5, 4, 15);
    g.addEdge(5, 6, 20);

    // Then
    Assertions::assertTrue(g.isDirected());
    Assertions::assertEquals(6LL, g.vertexCount());
    Assertions::assertEquals(10LL, g.edgeCount());
    Assertions::assertEquals(2LL, g.edgeCount(1));
    Assertions::assertEquals(-1LL, g.edgeCount(7));
    io::print(g);
};

auto should_register = []() {
    // Given
    graph::Graph<char> g;
    g.addVertex(1, 'a');
    g.addVertex(2, 'b');
    g.addVertex(3, 'c');
    g.addVertex(4, 'd');
    g.addVertex(5, 'e');
    g.addVertex(6, 'f');
    g.addEdge(1, 2, 1);
    g.addEdge(1, 3, 7);
    g.addEdge(2, 1, 15);
    g.addEdge(2, 3, 2);
    g.addEdge(2, 4, 5);
    g.addEdge(3, 4, 1);
    g.addEdge(3, 5, -5);
    g.addEdge(4, 6, 4);
    g.addEdge(5, 4, 15);
    g.addEdge(5, 6, 20);

    // When
    g.registerAlgorithm("filter_edge", [](const auto& g, auto&& args) -> util::DynArray<i64> {
        f64 min = opt<f64>(args, 0);
        f64 max = opt<f64>(args, 1);

        util::DynArray<i64> res;
        for (const auto& edge : g.edges()) {
            if (math::compare(edge.w, min) > 0 && math::compare(edge.w, max) < 0) {
                res.append(edge.w);
            }
        }
        return res;
    });

    // When
    auto res = g.callAlgorithm<util::DynArray<i64>>("filter_edge", 5.0, 25.0);

    // Then
    Assertions::assertEquals("[7,15,15,20]"_cs, res.__str__());
};

void test_graph() {
    UnitTestGroup group{"test_graph"};

    group.addTest("should_create_graph", should_create_graph);
    group.addTest("should_register", should_register);

    group.startAll();
}

} // namespace my::test::test_graph

#endif // TEST_GRAPH_HPP