/**
 * @brief 图辅助工具
 * @author Ricky
 * @date 2025/1/23
 * @version 1.0
 */
#ifndef GRAPH_HELPER_HPP
#define GRAPH_HELPER_HPP

#include "math_utils.hpp"
#include "Vec.hpp"

namespace my::graph {

/**
 * @brief 默认ID类型
 */
using DefaultIdx = u64;

/**
 * @brief 边
 */
template <typename E = f64, typename Idx = DefaultIdx>
struct Edge : public Object<Edge<E>> {
    Idx to; // 终点ID
    E w;     // 边权

    using Self = Edge<E>;

    Edge(Idx to, const E& w = E{}) :
            to(to), w(w) {}

    [[nodiscard]] cmp_t __cmp__(const Self& other) const {
        if constexpr (math::FloatingPointType<E>) {
            return math::fcmp(this->w, other.w);
        } else if constexpr (Comparable<E>) {
            return this->w.__cmp__(other.w);
        } else {
            return this->w - other.w;
        }
    }

    [[nodiscard]] CString __str__() const {
        std::stringstream stream;
        stream << '(' << to << ',' << w << ')'; // TODO 边权输出存在问题，不能输出其他类型
        return CString{stream.str()};
    }
};

/**
 * @brief 节点
 */
template <typename N = f64, typename E = f64, typename Idx = DefaultIdx>
struct Node : public Object<Node<N, E>> {
    Idx id;                   // 节点ID，唯一
    N w;                      // 点权
    util::Vec<Edge<E>> edges; // 该节点对应的边。如果为有向图，则代表以这个节点为起点的边

    using Self = Node<N, E>;

    Node() = default;

    Node(Idx id, const N& w = N{}) :
            id(id), w(w) {}

    /**
     * @brief 获取出度
     */
    usize out_deg() const {
        return edges.size();
    }

    /**
     * @brief 使用权值为w的边连接两个节点，若两个节点已连接，则不做任何事
     * @return true=两节点未连接 false=两节点已连接
     */
    bool connect(Idx to, const E& w) {
        for (auto&& edge : edges) {
            if (edge.to == to) {
                return false;
            }
        }
        edges.append(Edge{to, w});
        return true;
    }

    /**
     * @brief 删除连接两节点的边，若两节点之间没有边，则什么都不做
     * @return true=两节点已连接 false=两节点未连接
     */
    bool disconnect(Idx to) {
        auto size = edges.size();
        for (usize i = 0; i < size; ++i) {
            if (edges[i].to == to) {
                edges.pop(i);
                return true;
            }
        }
        return false;
    }

    /**
     * @brief 判断两节点是否连接
     * @return true=是 false=否
     */
    bool is_connected(Idx to) const {
        for (const auto& edge : edges) {
            if (edge.to == to) {
                return true;
            }
        }
        return false;
    }

    /**
     * @brief 遍历邻接边
     * @param C 消费者类型（接受两个参数：Idx id, E weight）
     */
    template <typename C>
    void for_each(C&& consumer) {
        for (auto& edge : edges) {
            std::forward<C>(consumer)(edge.to, edge.w);
        }
    }

    /**
     * @brief 遍历邻接边，常量版本
     * @param C 消费者类型（接受两个参数：Idx id, E weight）
     */
    template <typename C>
    void for_each(C&& consumer) const {
        for (const auto& edge : edges) {
            std::forward<C>(consumer)(edge.to, edge.w);
        }
    }

    [[nodiscard]] cmp_t __cmp__(const Self& other) const {
        if constexpr (math::FloatingPointType<N>) {
            return math::fcmp(this->w, other.w);
        } else if constexpr (Comparable<N>) {
            return this->w.__cmp__(other.w);
        } else {
            return this->w - other.w;
        }
    }

    [[nodiscard]] CString __str__() const {
        std::stringstream stream;
        stream << '(' << id << ',' << w << ")->"; // TODO 点权输出有问题，不能适配其他类型
        if (edges.empty()) {
            stream << "null";
            return CString{stream.str()};
        }

        bool first = true;
        for (const auto& edge : edges) {
            if (!first) stream << "->";
            stream << edge.__str__().data();
            first = false;
        }
        return CString{stream.str()};
    }
};

/**
 * @brief 简单路径，点集
 */
template <typename Idx = DefaultIdx>
struct SimplePath : public Object<SimplePath<Idx>> {
    util::Vec<Idx> nodes{}; // 点集

    /**
     * @brief 添加顶点
     */
    void append_node(Idx node_id) {
        nodes.append(node_id);
    }

    /**
     * @brief 弹出顶点
     */
    void pop_node() {
        nodes.pop();
    }

    /**
     * @brief 请空点集
     */
    void clear() {
        nodes.clear();
    }

    [[nodiscard]] CString __str__() const {
        return nodes.__str__();
    }
};

} // namespace my::graph

#endif // GRAPH_HELPER_HPP