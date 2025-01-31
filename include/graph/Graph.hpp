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
#include <shared_mutex>
#include <typeinfo>

namespace my::graph {

/**
 * @brief 图
 * @param ID 节点ID类型
 * @param V 点权类型
 * @param E 边权类型
 */
template <typename V = f64, typename E = f64>
class Graph : public Object<Graph<V, E>> {
    using self = Graph<V, E>;

public:
    explicit Graph(bool isDirected = true) :
            isDirected_(isDirected) {}

    c_size vertexCount() const {
        return vertices_.size();
    }

    c_size edgeCount() const {
        return edgeCount_;
    }

    /**
     * @brief 获取从ID节点出发的边的数量
     * @return 若节点不存在，则返回-1
     */
    c_size edgeCount(i64 id) const {
        if (!vertices_.contains(id)) {
            return -1;
        }
        return vertices_.get(id).outDegree();
    }

    /**
     * @brief 判断是否为有向图
     * @return true=是 false=否
     */
    bool isDirected() const {
        return isDirected_;
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
     * @brief 添加节点，若节点已存在，则什么都不做
     * @return true=节点不存在 false=节点已存在
     */
    bool addVertex(i64 id, const V& weight = V{}) {
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
    bool addEdge(i64 from, i64 to, const E& weight = E{}) {
        if (!vertices_.contains(from) || !vertices_.contains(to)) {
            ValueError(std::format("Node from[{}] or to[{}] does not exist.", from, to));
            return None<bool>;
        }
        auto tag = vertices_.get(from).connect(to, weight);
        ++edgeCount_;

        if (!isDirected_) {
            vertices_.get(to).connect(from, weight);
            ++edgeCount_;
        }
        return tag;
    }

    /**
     * @brief 注册算法插件
     * @param name 插件名称，若重复则覆盖已有算法
     * @param func 函数，例如 [](auto& g, auto&& args) {}
     */
    template <typename Func>
    void registerAlgorithm(const CString& name, Func&& func) {
        std::unique_lock lock(algoMutex_); // 写锁
        algorithms_.insert(name, [func = std::forward<Func>(func)](const Graph& g, util::DynArray<std::any> args) -> std::any {
            return func(g, std::move(args));
        });
    }

    /**
     * @brief 调用算法插件
     * @param ResultType 返回值类型，默认void
     * @param name 插件名称，若不存在则抛出ValueError
     * @param args 插件入参包
     */
    template <typename ResultType = void, typename... Args>
    ResultType callAlgorithm(const CString& name, Args&&... args) {
        std::shared_lock lock(algoMutex_); // 读锁
        if (!algorithms_.contains(name)) {
            ValueError(std::format("Algorithm[{}] not found.", name));
        }
        auto algorithm = algorithms_.get(name);
        return std::any_cast<ResultType>(algorithm(*this, {std::any(std::forward<Args>(args))...}));
    }

    template <typename ResultType = void, typename... Args>
    ResultType callAlgorithm(const CString& name, Args&&... args) const {
        std::shared_lock lock(algoMutex_); // 读锁
        if (!algorithms_.contains(name)) {
            ValueError(std::format("Algorithm[{}] not found.", name));
        }
        auto algorithm = algorithms_.get(name);
        return std::any_cast<ResultType>(algorithm(*this, {std::any(std::forward<Args>(args))...}));
    }

    CString __str__() const {
        std::stringstream stream;
        stream << "Graph (" << (isDirected_ ? "Directed" : "Undirected") << ")\n";
        stream << "Vertex count: " << vertexCount() << '\n';
        stream << "Edge count: " << edgeCount() << '\n';
        stream << "Vertex value type: " << typeid(V).name() << '\n';
        stream << "Edge value type: " << typeid(E).name() << '\n';
        stream << "Adjacency List:\n";

        for (const auto& vertex : vertices_.values()) {
            stream << "[" << vertex.id << "](" << vertex.w << ") -> ";
            bool first = true;
            for (const auto& edge : vertex.edges) {
                if (!first) stream << " -> ";
                stream << "[" << edge.end << "](" << edge.w << ")";
                first = false;
            }
            if (vertex.edges.empty()) {
                stream << "null";
            }
            stream << '\n';
        }
        return CString(stream.str());
    }

private:
    c_size edgeCount_ = 0;                   // 边数，无向图为双倍边
    bool isDirected_;                        // 是否为有向图 true=是 false=否
    util::Dict<i64, Vertex<V, E>> vertices_; // 邻接表

    // 插件系统
    using Algorithm = std::function<std::any(Graph&, util::DynArray<std::any>)>;
    mutable util::Dict<CString, Algorithm> algorithms_;
    mutable std::shared_mutex algoMutex_;
};

} // namespace my::graph

#endif // GRAPH_HPP