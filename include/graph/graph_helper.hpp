/**
 * @brief 图辅助工具
 * @author Ricky
 * @date 2025/1/23
 * @version 1.0
 */
#ifndef GRAPH_HELPER_HPP
#define GRAPH_HELPER_HPP

#include "math_utils.hpp"
#include "DynArray.hpp"

namespace my::graph {

/**
 * @brief 边
 */
template <typename E = f64>
struct Edge : public Object<Edge<E>> {
    u64 end; // 终点ID
    E w;     // 边权

    using Self = Edge<E>;

    Edge(u64 end, const E& w = E{}) :
            end(end), w(w) {}

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
        stream << '(' << end << ',' << w << ')';
        return CString{stream.str()};
    }
};

/**
 * @brief 节点
 */
template <typename V = f64, typename E = f64>
struct Vertex : public Object<Vertex<V, E>> {
    u64 id;                        // 节点ID，唯一
    V w;                           // 点权
    util::DynArray<Edge<E>> edges; // 该节点对应的边。如果为有向图，则代表以这个节点为起点的边

    using Self = Vertex<V, E>;

    Vertex() = default;

    Vertex(u64 id, const V& w = V{}) :
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
    bool connect(u64 end, const E& w) {
        for (auto&& edge : edges) {
            if (edge.end == end) {
                return false;
            }
        }
        edges.append(Edge{end, w});
        return true;
    }

    /**
     * @brief 删除连接两节点的边，若两节点之间没有边，则什么都不做
     * @return true=两节点已连接 false=两节点未连接
     */
    bool disconnect(u64 end) {
        auto size = edges.size();
        for (usize i = 0; i < size; ++i) {
            if (edges[i].end == end) {
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
    bool is_connected(u64 end) const {
        for (const auto& edge : edges) {
            if (edge.end == end) {
                return true;
            }
        }
        return false;
    }

    /**
     * @brief 遍历邻接边
     */
    template <typename C>
    void for_each(C&& consumer) {
        for (auto& edge : edges) {
            std::forward<C>(consumer)(edge);
        }
    }

    /**
     * @brief 遍历邻接边，常量版本
     */
    template <typename C>
    void for_each(C&& consumer) const {
        for (const auto& edge : edges) {
            std::forward<C>(consumer)(edge);
        }
    }

    [[nodiscard]] cmp_t __cmp__(const Self& other) const {
        if constexpr (math::FloatingPointType<V>) {
            return math::fcmp(this->w, other.w);
        } else if constexpr (Comparable<V>) {
            return this->w.__cmp__(other.w);
        } else {
            return this->w - other.w;
        }
    }

    [[nodiscard]] CString __str__() const {
        std::stringstream stream;
        stream << '(' << id << ',' << w << ")->";
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

} // namespace my::graph

#endif // GRAPH_HELPER_HPP