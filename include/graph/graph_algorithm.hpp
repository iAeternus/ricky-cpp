/**
 * @brief 图算法
 * @author Ricky
 * @date 2025/1/23
 * @version 1.0
 */
#ifndef GRAPH_ALGORITHM_HPP
#define GRAPH_ALGORITHM_HPP

#include "Matrix.hpp"

namespace my::graph {

#define INF F64_MAX

/**
 * @brief 样板算法
 */
template <typename V = f64, typename E = f64>
auto fake_algorithm = [](const auto& g, auto&& args) -> CString {
    auto a = opt<i32>(args, 0);
    auto b = opt<i32>(args, 1);
    auto c = opt<i32>(args, 2);

    auto edges = g.edges();
    io::println(edges);

    return std::format("Fake algorithm. Args are {},{},{}", a, b, c);
};

/**
 * @brief 邻接表转邻接矩阵
 * @note 会丢失点权信息，要求顶点id从0开始
 */
template <typename V = f64, typename E = f64>
auto adj2matrix = [](const auto& g, auto&& args) -> math::Matrix<E> {
    math::Matrix<E> m(g.vertex_cnt(), INF);
    g.for_each([&](const auto& vertex) {
        m[vertex.id][vertex.id] = 0;
        vertex.for_each([&](const auto& edge) {
            m[vertex.id][edge.end] = edge.w;
        });
    });
    return m;
};

/**
 * @brief 判断图中是否存在EL路径
 * @note EL路径：包含所有边且长度为|E|的路径
 * @note 判断方式：度数为奇数的顶点个数为不大于2的偶数
 * @note 时间复杂度：O(|V|^2 * |E|)，若用邻接矩阵则为 O(|V|^2)
 */
template <typename V = f64, typename E = f64>
auto is_exist_el = [](const auto& g, auto&& args) -> bool {
    isize cnt = 0;
    g.for_each([&](const auto& vertex) {
        isize deg = vertex.out_deg() + g.in_deg(vertex.id);
        if (deg & 1) {
            ++cnt;
        }
    });
    return cnt == 0 || cnt == 2;
};

} // namespace my::graph

#endif // GRAPH_ALGORITHM_HPP