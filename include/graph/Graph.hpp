/**
 * @brief 图
 * @author Ricky
 * @date 2025/1/23
 * @version 1.0
 */
#ifndef GRAPH_HPP
#define GRAPH_HPP

#include "DisjointSet.hpp"

namespace my::util {

/**
 * @brief 图
 * @param ID 节点ID类型
 * @param V 点权类型
 * @param E 边权类型
 */
template <typename ID, typename V = f64, typename E = f64>
class Graph : public Object<Graph<ID, V, E>> {
    using self = Graph<ID, V, E>;
public:
private:
    c_size vertexCount_; // 节点数
    c_size edgeCount_;   // 边数
    bool isDirect;       // 是否为有向图 true=是 false=否
};

} // namespace my::util

#endif // GRAPH_HPP