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

#include <memory>
#include <shared_mutex>

namespace my::graph {

template <typename N, typename E, typename Idx>
class Graph;

/**
 * @brief 基类算法接口，用于类型擦除
 */
template <typename N, typename E, typename Idx>
class AlgorithmBase {
public:
    virtual ~AlgorithmBase() = default;
    virtual std::any execute(const Graph<N, E, Idx>& g, util::Vec<std::any>& args) const = 0;
};

/**
 * @brief 图
 * @param N 点权类型
 * @param E 边权类型
 */
template <typename N = f64, typename E = f64, typename Idx = DefaultIdx>
class Graph : public Object<Graph<N, E>> {
public:
    using Self = Graph<N, E>;

    explicit Graph(bool is_directed = true) :
            is_directed_(is_directed) {}

    /**
     * @brief 对于拷贝，不继承算法字典
     */
    Graph(const Self& other) :
            edge_cnt_(other.edge_cnt_), is_directed_(other.is_directed_), nodes_(other.nodes_) {}

    Self& operator=(const Self& other) {
        if (this == &other) return *this;

        this->edge_cnt_ = other.edge_cnt_;
        this->is_directed_ = other.is_directed_;
        this->nodes_ = other.nodes_;
        return *this;
    }

    usize node_cnt() const {
        return nodes_.size();
    }

    usize edge_cnt() const {
        return edge_cnt_;
    }

    bool has_node(Idx id) const {
        return nodes_.contains(id);
    }

    bool has_edge(Idx from, Idx to) const {
        return get_node(from).is_connected(to);
    }

    /**
     * @brief 获取从ID节点出发的边的数量
     * @return 若节点不存在，则返回npos
     */
    usize edge_cnt(Idx id) const {
        if (!has_node(id)) {
            return npos;
        }
        return nodes_.get(id).out_deg();
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
    Node<N, E>& get_node(Idx id) {
        return nodes_.get(id);
    }

    Node<N, E> get_node(Idx id) const {
        return nodes_.get(id);
    }

    /**
     * @brief 获取节点的迭代视图
     */
    auto nodes() const {
        return nodes_.values();
    }

    /**
     * @brief 获取边的迭代视图
     */
    auto edges() const {
        util::Vec<Edge<E>> edges;
        for (const auto& vertex : nodes_.values()) {
            for (const auto& edge : vertex.edges) {
                edges.append(edge);
            }
        }
        return edges;
    }

    /**
     * @brief 获取节点的入度
     * @param id 节点ID
     * @note 时间复杂度：O(|N| * |E|)
     */
    usize in_deg(Idx id) const {
        usize deg = 0;
        for_each([&](const auto& vertex) {
            if (id == vertex.id) return;
            vertex.for_each([&](Idx v, E _) {
                if (id == v) {
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
        for (auto& [id, vertex] : nodes_) {
            std::forward<C>(consumer)(vertex);
        }
    }

    /**
     * @brief 遍历所有顶点，常量版本
     */
    template <typename C>
    void for_each(C&& consumer) const {
        for (const auto& [id, vertex] : nodes_) {
            std::forward<C>(consumer)(vertex);
        }
    }

    /**
     * @brief 添加节点，若节点已存在，则什么都不做
     * @return true=节点不存在 false=节点已存在
     */
    bool add_node(Idx id, const N& weight = N{}) {
        if (has_node(id)) {
            return false;
        }
        nodes_.insert(id, Node{id, weight});
        return true;
    }

    /**
     * @brief 添加边，若某一点不存在，则抛出not_found_exception，若边已存在，则什么都不做
     * @return true=边不存在 false=边已存在
     */
    bool add_edge(Idx from, Idx to, const E& weight = E{}) {
        if (!has_node(from) || !has_node(to)) {
            throw not_found_exception("node from[{}] or to[{}] does not exist.", SRC_LOC, from, to);
        }
        auto tag = nodes_.get(from).connect(to, weight);
        ++edge_cnt_;

        if (!is_directed_) {
            nodes_.get(to).connect(from, weight);
            ++edge_cnt_;
        }
        return tag;
    }

private:
    /**
     * @brief 算法包装器，直接调用函数
     */
    template <typename Func>
    class AlgorithmWrapper : public AlgorithmBase<N, E, Idx> {
    public:
        explicit AlgorithmWrapper(const Func& func) :
                func_(func) {}

        std::any execute(const Graph<N, E>& g, util::Vec<std::any>& args) const override {
            if constexpr (std::is_void_v<decltype(func_(g, args))>) {
                func_(g, args); // 无返回值类型
                return {};
            } else {
                return std::any(func_(g, args)); // 有返回值类型
            }
        }

    private:
        Func func_;
    };

public:
    /**
     * @brief 注册算法插件
     * @param name 插件名称，若重复则覆盖已有算法
     * @param func 算法函数对象
     */
    template <typename Func>
    void register_algo(const CString& name, const Func& func) {
        std::unique_lock lock(algo_mutex_);
        algorithms_.insert(name, std::make_shared<AlgorithmWrapper<Func>>(func));
    }

    /**
     * @brief 调用算法插件
     * @param RetType 返回值类型
     * @param name 插件名称，若不存在则抛出not_found_exception
     * @param args 插件入参包
     */
    template <typename RetType = void, typename... Args>
    RetType call_algo(const CString& name, Args&&... args) const {
        std::shared_lock lock(algo_mutex_);

        if (!algorithms_.contains(name)) {
            throw not_found_exception("algorithm[{}] not found.", SRC_LOC, name);
        }

        const auto& algorithm = algorithms_.get(name);
        util::Vec<std::any> args_pack = {std::any(std::forward<Args>(args))...};
        auto result = algorithm->execute(*this, args_pack);

        if constexpr (std::is_same_v<RetType, void>) {
            return;
        } else {
            return std::any_cast<RetType>(result);
        }
    }

    [[nodiscard]] CString __str__() const {
        std::stringstream stream;
        stream << "Graph (" << (is_directed_ ? "Directed" : "Undirected") << ")\n";
        stream << "Node count: " << node_cnt() << '\n';
        stream << "Edge count: " << (is_directed_ ? edge_cnt() : edge_cnt() >> 1) << '\n';
        stream << "Node value type: " << dtype(N) << '\n';
        stream << "Edge value type: " << dtype(E) << '\n';
        stream << "Adjacency List:\n";

        for (const auto& vertex : nodes_.values()) {
            stream << vertex.__str__().data() << '\n';
        }
        return CString{stream.str()};
    }

private:
    usize edge_cnt_ = 0;                // 边数，无向图为双倍边
    bool is_directed_;                  // 是否为有向图 true=是 false=否
    util::Dict<Idx, Node<N, E>> nodes_; // 邻接表

    // 算法插件系统
    mutable util::Dict<CString, std::shared_ptr<AlgorithmBase<N, E, Idx>>> algorithms_{};
    mutable std::shared_mutex algo_mutex_{};
};

template <typename N = f64, typename E = f64, typename Idx = DefaultIdx>
using Tree = Graph<N, E, Idx>;

} // namespace my::graph

#endif // GRAPH_HPP