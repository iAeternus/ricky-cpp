/**
 * @brief 图算法
 * @author Ricky
 * @date 2025/1/23
 * @version 1.0
 */
#ifndef GRAPH_ALGORITHM_HPP
#define GRAPH_ALGORITHM_HPP

#include "Graph.hpp"

namespace my::graph {

/**
 * @brief 样板算法
 */
template <typename V = f64, typename E = f64>
auto fake_algorithm = [](const Graph<V, E>& g, util::DynArray<std::any>&& args) -> CString {
    auto a = opt<i32>(args, 0);
    auto b = opt<i32>(args, 1);
    auto c = opt<i32>(args, 2);

    return std::format("Fake algorithm. Args are {},{},{}", a, b, c);
};

} // namespace my::graph

#endif // GRAPH_ALGORITHM_HPP