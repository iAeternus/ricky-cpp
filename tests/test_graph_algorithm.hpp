#ifndef TEST_GRAPH_ALGORITHM_HPP
#define TEST_GRAPH_ALGORITHM_HPP

#include "ricky_test.hpp"
#include "graph_algorithm.hpp"

namespace my::test::test_graph_algorithm {

auto test_fake_algorithm = []() {
    // Given
    graph::Graph g;
    g.register_algo("fake_algorithm", graph::fake_algorithm<f64, f64>);

    // When
    auto res = g.call_algo<CString>("fake_algorithm", 1, 2, 3);

    // Then
    Assertions::assertEquals("Fake algorithm. Args are 1,2,3"_cs, res);
};

auto test_adj2matrix = []() {
    // Given
    graph::Graph g;
    g.add_vertex(0);
    g.add_vertex(1);
    g.add_vertex(2);
    g.add_vertex(3);
    g.add_vertex(4);
    g.add_vertex(5);
    g.add_vertex(6);
    g.add_edge(0, 1, 15);
    g.add_edge(0, 2, 2);
    g.add_edge(0, 3, 12);
    g.add_edge(1, 4, 6);
    g.add_edge(2, 4, 8);
    g.add_edge(2, 5, 4);
    g.add_edge(3, 6, 3);
    g.add_edge(4, 6, 9);
    g.add_edge(5, 3, 5);
    g.add_edge(5, 6, 10);
    g.add_edge(6, 1, 4);

    g.register_algo("adj2matrix", graph::adj2matrix<f64, f64>);

    // When
    auto res = g.call_algo<math::Matrix<f64>>("adj2matrix");

    // Then
    using namespace my::graph;
    Assertions::assertEquals(math::Matrix<f64>{
                                 {0, 15, 2, 12, INF, INF, INF},
                                 {INF, 0, INF, INF, 6, INF, INF},
                                 {INF, INF, 0, INF, 8, 4, INF},
                                 {INF, INF, INF, 0, INF, INF, 3},
                                 {INF, INF, INF, INF, 0, INF, 9},
                                 {INF, INF, INF, 5, INF, 0, 10},
                                 {INF, 4, INF, INF, INF, INF, 0}},
                             res);
};

auto test_is_exist_el = []() {
    // Given
    graph::Graph g;
    g.add_vertex(0);
    g.add_vertex(1);
    g.add_vertex(2);
    g.add_vertex(3);
    g.add_vertex(4);
    g.add_vertex(5);
    g.add_vertex(6);
    g.add_edge(0, 1, 1);
    g.add_edge(1, 2, 1);
    g.add_edge(2, 3, 1);
    g.add_edge(3, 4, 1);
    g.add_edge(4, 5, 1);
    g.add_edge(5, 6, 1);
    g.add_edge(6, 0, 1);

    graph::Graph g2;
    g2.add_vertex(0);
    g2.add_vertex(1);
    g2.add_vertex(2);
    g2.add_vertex(3);
    g2.add_vertex(4);
    g2.add_vertex(5);
    g2.add_vertex(6);
    g2.add_edge(0, 1, 15);
    g2.add_edge(0, 2, 2);
    g2.add_edge(0, 3, 12);
    g2.add_edge(1, 4, 6);
    g2.add_edge(2, 4, 8);
    g2.add_edge(2, 5, 4);
    g2.add_edge(3, 6, 3);
    g2.add_edge(4, 6, 9);
    g2.add_edge(5, 3, 5);
    g2.add_edge(5, 6, 10);
    g2.add_edge(6, 1, 4);

    g.register_algo("is_exist_el", graph::is_exist_el<f64, f64>);
    g2.register_algo("is_exist_el", graph::is_exist_el<f64, f64>);

    // When
    auto res = g.call_algo<bool>("is_exist_el");
    auto res2 = g2.call_algo<bool>("is_exist_el");

    // Then
    Assertions::assertTrue(res);
    Assertions::assertFalse(res2);
};

inline void test_graph_algorithm() {
    UnitTestGroup group{"test_graph_algorithm"};

    group.addTest("test_fake_algorithm", test_fake_algorithm);
    group.addTest("test_adj2matrix", test_adj2matrix);
    group.addTest("test_is_exist_el", test_is_exist_el);

    group.startAll();
}

} // namespace my::test::test_graph_algorithm

#endif // TEST_GRAPH_ALGORITHM_HPP