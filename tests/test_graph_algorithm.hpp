#ifndef TEST_GRAPH_ALGORITHM_HPP
#define TEST_GRAPH_ALGORITHM_HPP

#include "Assertions.hpp"
#include "graph_helper.hpp"
#include "UnitTest.hpp"
#include "Assertions.hpp"
#include "Vec.hpp"
#include "graph_algorithm.hpp"
#include "Graph.hpp"

namespace my::test::test_graph_algorithm {

fn test_adj2matrix = []() {
    // Given
    graph::Graph g;
    g.add_node(0);
    g.add_node(1);
    g.add_node(2);
    g.add_node(3);
    g.add_node(4);
    g.add_node(5);
    g.add_node(6);
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

    g.register_algo("adj2matrix", graph::adj2matrix<>);

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

fn test_is_exist_el = []() {
    // Given
    graph::Graph g;
    g.add_node(0);
    g.add_node(1);
    g.add_node(2);
    g.add_node(3);
    g.add_node(4);
    g.add_node(5);
    g.add_node(6);
    g.add_edge(0, 1, 1);
    g.add_edge(1, 2, 1);
    g.add_edge(2, 3, 1);
    g.add_edge(3, 4, 1);
    g.add_edge(4, 5, 1);
    g.add_edge(5, 6, 1);
    g.add_edge(6, 0, 1);

    graph::Graph g2;
    g2.add_node(0);
    g2.add_node(1);
    g2.add_node(2);
    g2.add_node(3);
    g2.add_node(4);
    g2.add_node(5);
    g2.add_node(6);
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

    g.register_algo("is_exist_el", graph::is_exist_el<>);
    g2.register_algo("is_exist_el", graph::is_exist_el<>);

    // When
    auto res = g.call_algo<bool>("is_exist_el");
    auto res2 = g2.call_algo<bool>("is_exist_el");

    // Then
    Assertions::assertTrue(res);
    Assertions::assertFalse(res2);
};

fn should_bfs = []() {
    // Given
    graph::Graph g;
    g.add_node(0);
    g.add_node(1);
    g.add_node(2);
    g.add_node(3);
    g.add_node(4);
    g.add_node(5);
    g.add_node(6);
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

    util::Vec<u64> vis_order;
    Consumer<const graph::Node<f64, f64>&> callback = [&vis_order](const graph::Node<>& node) {
        vis_order.append(node.id);
    };

    // When
    g.register_algo("bfs", graph::bfs<>);
    g.call_algo("bfs", 0ULL, callback);

    // Then
    Assertions::assertEquals("[0,1,2,3,4,5,6]"_cs, vis_order.__str__());
};

fn should_dfs = []() {
    // Given
    graph::Graph g;
    g.add_node(0);
    g.add_node(1);
    g.add_node(2);
    g.add_node(3);
    g.add_node(4);
    g.add_node(5);
    g.add_node(6);
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

    util::Vec<u64> vis_order;
    Consumer<const graph::Node<f64, f64>&> callback = [&vis_order](const graph::Node<>& node) {
        vis_order.append(node.id);
    };

    // When
    g.register_algo("dfs", graph::dfs<>);
    g.call_algo("dfs", 0ULL, callback);

    // Then
    Assertions::assertEquals("[0,1,4,6,2,5,3]"_cs, vis_order.__str__());
};

fn test_is_tree = []() {
    // Given
    graph::Graph g(false);
    g.add_node(0);
    g.add_node(1);
    g.add_node(2);
    g.add_node(3);
    g.add_node(4);
    g.add_node(5);
    g.add_node(6);
    g.add_edge(0, 1);
    g.add_edge(0, 2);
    g.add_edge(0, 3);
    g.add_edge(1, 4);
    g.add_edge(1, 5);
    g.add_edge(3, 6);

    // When
    g.register_algo("is_tree", graph::is_tree<>);
    bool res = g.call_algo<bool>("is_tree");

    // Then
    Assertions::assertTrue(res);
};

fn test_can_reach = []() {
    // Given
    graph::Graph g;
    g.add_node(0);
    g.add_node(1);
    g.add_node(2);
    g.add_node(3);
    g.add_node(4);
    g.add_node(5);
    g.add_node(6);
    g.add_edge(0, 1);
    g.add_edge(0, 2);
    g.add_edge(0, 3);
    g.add_edge(1, 4);
    g.add_edge(1, 5);
    g.add_edge(3, 6);

    // When
    g.register_algo("can_reach_dfs", graph::can_reach_dfs<>);
    g.register_algo("can_reach_bfs", graph::can_reach_bfs<>);

    bool res_dfs = g.call_algo<bool>("can_reach_dfs", 0ULL, 5ULL);
    bool res_dfs2 = g.call_algo<bool>("can_reach_dfs", 5ULL, 6ULL);

    bool res_bfs = g.call_algo<bool>("can_reach_bfs", 0ULL, 5ULL);
    bool res_bfs2 = g.call_algo<bool>("can_reach_bfs", 5ULL, 6ULL);

    // Then
    Assertions::assertTrue(res_dfs);
    Assertions::assertFalse(res_dfs2);

    Assertions::assertTrue(res_bfs);
    Assertions::assertFalse(res_bfs2);
};

fn should_get_all_paths = []() {
    // Given
    graph::Graph g;
    g.add_node(0);
    g.add_node(1);
    g.add_node(2);
    g.add_node(3);
    g.add_node(4);
    g.add_node(5);
    g.add_edge(0, 1);
    g.add_edge(0, 2);
    g.add_edge(0, 3);
    g.add_edge(1, 3);
    g.add_edge(2, 3);
    g.add_edge(3, 4);
    g.add_edge(3, 5);
    g.add_edge(4, 5);

    // When
    g.register_algo("get_all_paths", graph::get_all_paths<>);
    auto paths = g.call_algo<util::Vec<graph::SimplePath<>>>("get_all_paths", 0ULL, 5ULL);

    // Then
    Assertions::assertEquals("[[0,1,3,4,5],[0,1,3,5],[0,2,3,4,5],[0,2,3,5],[0,3,4,5],[0,3,5]]"_cs, paths.__str__());
};

fn test_prim = []() {
    // Given
    graph::Graph<char> g(false);
    g.add_node(0, '1');
    g.add_node(1, '2');
    g.add_node(2, '3');
    g.add_node(3, '4');
    g.add_node(4, '5');
    g.add_node(5, '6');
    g.add_edge(0, 1, 6);
    g.add_edge(0, 2, 1);
    g.add_edge(0, 3, 5);
    g.add_edge(1, 2, 5);
    g.add_edge(1, 4, 3);
    g.add_edge(2, 3, 5);
    g.add_edge(2, 4, 6);
    g.add_edge(2, 5, 4);
    g.add_edge(3, 5, 2);
    g.add_edge(4, 5, 6);

    // When
    g.register_algo("prim", graph::prim<char>);
    g.register_algo("prim2", graph::prim2<char>);
    // g.register_algo("kruskal", graph::kruskal<char>);
    auto t = g.call_algo<graph::Tree<char>>("prim");
    auto t2 = g.call_algo<graph::Tree<char>>("prim2");
    // auto t3 = g.call_algo<graph::Tree<char>>("kruskal");

    // Then
    Assertions::assertEquals(6, t.node_cnt());
    Assertions::assertEquals(5, t.edge_cnt() / 2);
    Assertions::assertTrue(t.has_edge(0, 2));
    Assertions::assertTrue(t.has_edge(1, 2));
    Assertions::assertTrue(t.has_edge(1, 4));
    Assertions::assertTrue(t.has_edge(2, 5));
    Assertions::assertTrue(t.has_edge(3, 5));

    Assertions::assertEquals(6, t2.node_cnt());
    Assertions::assertEquals(5, t2.edge_cnt() / 2);
    Assertions::assertTrue(t2.has_edge(0, 2));
    Assertions::assertTrue(t2.has_edge(1, 2));
    Assertions::assertTrue(t2.has_edge(1, 4));
    Assertions::assertTrue(t2.has_edge(2, 5));
    Assertions::assertTrue(t2.has_edge(3, 5));

    // Assertions::assertEquals(6, t3.node_cnt());
    // Assertions::assertEquals(5, t3.edge_cnt() / 2);
    // Assertions::assertTrue(t3.has_edge(0, 2));
    // Assertions::assertTrue(t3.has_edge(1, 2));
    // Assertions::assertTrue(t3.has_edge(1, 4));
    // Assertions::assertTrue(t3.has_edge(2, 5));
    // Assertions::assertTrue(t3.has_edge(3, 5));
};

fn test_dijkstra = []() {
    // Given
    graph::Graph<char> g(false);
    g.add_node(0, 'A');
    g.add_node(1, 'B');
    g.add_node(2, 'C');
    g.add_node(3, 'D');
    g.add_node(4, 'E');
    g.add_node(5, 'F');
    g.add_node(6, 'G');

    g.add_edge(0, 1, 12);
    g.add_edge(0, 5, 16);
    g.add_edge(0, 6, 14);
    g.add_edge(1, 2, 10);
    g.add_edge(1, 5, 7);
    g.add_edge(2, 3, 3);
    g.add_edge(2, 4, 5);
    g.add_edge(2, 5, 6);
    g.add_edge(3, 4, 4);
    g.add_edge(4, 5, 2);
    g.add_edge(4, 6, 8);
    g.add_edge(5, 6, 9);

    // When
    g.register_algo("dijkstra", graph::dijkstra<char>);
    auto dis = g.call_algo<util::Vec<f64>>("dijkstra", 0ULL);

    // Then
    Assertions::assertEquals("[0,12,22,22,18,16,14]"_cs, dis.__str__());
};

fn test_graph_algorithm() {
    UnitTestGroup group{"test_graph_algorithm"};

    group.addTest("test_adj2matrix", test_adj2matrix);
    group.addTest("test_is_exist_el", test_is_exist_el);
    group.addTest("should_bfs", should_bfs);
    group.addTest("should_dfs", should_dfs);
    group.addTest("test_is_tree", test_is_tree);
    group.addTest("test_can_reach", test_can_reach);
    group.addTest("should_get_all_paths", should_get_all_paths);
    group.addTest("test_prim", test_prim);
    group.addTest("test_dijkstra", test_dijkstra);

    group.startAll();
}

} // namespace my::test::test_graph_algorithm

#endif // TEST_GRAPH_ALGORITHM_HPP