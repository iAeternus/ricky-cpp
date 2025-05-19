/**
 * @brief 图
 * @author Ricky
 * @date 2025/1/23
 * @version 1.0
 */
#ifndef GRAPH_HPP
#define GRAPH_HPP

#include "graph_helper.hpp"
#include "Dict.hpp"

#include <any>
#include <functional>
#include <shared_mutex>

namespace my::graph {

/**
 * @brief 图
 * @param ID 节点ID类型
 * @param V 点权类型
 * @param E 边权类型
 */
template <typename V = f64, typename E = f64>
class Graph : public Object<Graph<V, E>> {
public:
    using Self = Graph<V, E>;
    using Algorithm = std::function<std::any(Graph&, util::DynArray<std::any>&&)>;

    explicit Graph(bool is_directed = true) :
            is_directed_(is_directed) {}

    usize vertex_cnt() const {
        return vertices_.size();
    }

    usize edge_cnt() const {
        return edge_cnt_;
    }

    /**
     * @brief 获取从ID节点出发的边的数量
     * @return 若节点不存在，则返回npos
     */
    usize edge_cnt(u64 id) const {
        if (!vertices_.contains(id)) {
            return npos;
        }
        return vertices_.get(id).out_deg();
    }

    /**
     * @brief 判断是否为有向图
     * @return true=是 false=否
     */
    bool is_directed() const {
        return is_directed_;
    }

    /**
     * @brief 根据ID获取节点
     */
    Vertex<V, E>& get_vertex(u64 id) {
        return vertices_.get(id);
    }

    /**
     * @brief 获取节点的迭代视图
     */
    auto vertices() const {
        return vertices_.values();
    }

    /**
     * @brief 获取边的迭代视图
     */
    auto edges() const {
        util::DynArray<Edge<E>> edges;
        for (const auto& vertex : vertices_.values()) {
            for (const auto& edge : vertex.edges) {
                edges.append(edge);
            }
        }
        return edges;
    }

    /**
     * @brief 获取节点的入度
     * @param id 节点ID
     * @note 时间复杂度：O(|V| * |E|)
     */
    usize in_deg(u64 id) const {
        usize deg = 0;
        for_each([&](const auto& vertex) {
            if (id == vertex.id) return;
            vertex.for_each([&](const auto& edge) {
                if (id == edge.end) {
                    ++deg;
                }
            });
        });
        return deg;
    }

    /**
     * @brief 遍历所有顶点
     */
    template <typename C>
    void for_each(C&& consumer) {
        for (auto& [id, vertex] : vertices_) {
            std::forward<C>(consumer)(vertex);
        }
    }

    /**
     * @brief 遍历所有顶点，常量版本
     */
    template <typename C>
    void for_each(C&& consumer) const {
        for (const auto& [id, vertex] : vertices_) {
            std::forward<C>(consumer)(vertex);
        }
    }

    /**
     * @brief 添加节点，若节点已存在，则什么都不做
     * @return true=节点不存在 false=节点已存在
     */
    bool add_vertex(u64 id, const V& weight = V{}) {
        if (vertices_.contains(id)) {
            return false;
        }
        vertices_.insert(id, Vertex{id, weight});
        return true;
    }

    /**
     * @brief 添加边，若某一点不存在，则抛出异常，若边已存在，则什么都不做
     * @return true=边不存在 false=边已存在
     */
    bool add_edge(u64 from, u64 to, const E& weight = E{}) {
        if (!vertices_.contains(from) || !vertices_.contains(to)) {
            ValueError(std::format("Node from[{}] or to[{}] does not exist.", from, to));
            std::unreachable();
        }
        auto tag = vertices_.get(from).connect(to, weight);
        ++edge_cnt_;

        if (!is_directed_) {
            vertices_.get(to).connect(from, weight);
            ++edge_cnt_;
        }
        return tag;
    }

    /**
     * @brief 注册算法插件
     * @param name 插件名称，若重复则覆盖已有算法
     * @param func 算法，例如 [](auto& g, auto&& args) {}
     */
    template <typename Func>
    void register_algo(const CString& name, Func&& func) {
        std::unique_lock lock(algo_mutex_); // 写锁
        algorithms_.insert(name, [func = std::forward<Func>(func)](const Graph& g, util::DynArray<std::any>&& args) -> std::any {
            return func(g, std::move(args));
        });
    }

    /**
     * @brief 调用算法插件
     * @param RetType 返回值类型，默认void
     * @param name 插件名称，若不存在则抛出ValueError
     * @param args 插件入参包
     */
    template <typename RetType = void, typename... Args>
    RetType call_algo(const CString& name, Args&&... args) {
        std::shared_lock lock(algo_mutex_); // 读锁
        if (!algorithms_.contains(name)) {
            ValueError(std::format("Algorithm[{}] not found.", name));
        }
        auto algorithm = algorithms_.get(name);
        return std::any_cast<RetType>(algorithm(*this, {std::any(std::forward<Args>(args))...}));
    }

    template <typename RetType = void, typename... Args>
    RetType call_algo(const CString& name, Args&&... args) const {
        std::shared_lock lock(algo_mutex_); // 读锁
        if (!algorithms_.contains(name)) {
            ValueError(std::format("Algorithm[{}] not found.", name));
        }
        auto algorithm = algorithms_.get(name);
        return std::any_cast<RetType>(algorithm(*this, {std::any(std::forward<Args>(args))...}));
    }

    [[nodiscard]] CString __str__() const {
        std::stringstream stream;
        stream << "Graph (" << (is_directed_ ? "Directed" : "Undirected") << ")\n";
        stream << "Vertex count: " << vertex_cnt() << '\n';
        stream << "Edge count: " << edge_cnt() << '\n';
        stream << "Vertex value type: " << dtype(V) << '\n';
        stream << "Edge value type: " << dtype(E) << '\n';
        stream << "Adjacency List:\n";

        for (const auto& vertex : vertices_.values()) {
            stream << vertex.__str__().data() << '\n';
        }
        return CString{stream.str()};
    }

private:
    usize edge_cnt_ = 0;                     // 边数，无向图为双倍边
    bool is_directed_;                       // 是否为有向图 true=是 false=否
    util::Dict<u64, Vertex<V, E>> vertices_; // 邻接表

    // 插件系统
    mutable util::Dict<CString, Algorithm> algorithms_;
    mutable std::shared_mutex algo_mutex_;
};

} // namespace my::graph

#endif // GRAPH_HPP