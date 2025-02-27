#ifndef TEST_GRAPH_ALGORITHM_HPP
#define TEST_GRAPH_ALGORITHM_HPP

#include "ricky_test.hpp"
#include "graph_algorithm.hpp"

namespace my::test::test_graph_algorithm {

auto test_fake_algorithm = []() {
    // Given
    graph::Graph g;
    g.registerAlgorithm("fake_algorithm", graph::fake_algorithm<f64, f64>);
    
    // When
    auto res = g.callAlgorithm<CString>("fake_algorithm", 1, 2, 3);

    // Then
    Assertions::assertEquals("Fake algorithm. Args are 1,2,3"_cs, res);
};
 
void test_graph_algorithm() {
    UnitTestGroup group{"test_graph_algorithm"};

    group.addTest("test_fake_algorithm", test_fake_algorithm);

    group.startAll();
}

} // namespace my::test::test_graph_algorithm

#endif // TEST_GRAPH_ALGORITHM_HPP