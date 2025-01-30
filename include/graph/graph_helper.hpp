#ifndef GRAPH_HELPER_HPP
#define GRAPH_HELPER_HPP

namespace my::graph {

/**
     * @brief 边
     */
    template <typename ID, typename V = f64, typename E = f64>
struct Edge : public Object<Edge> {
private:
    using self = Edge;

public:
    ID begin; // 起始点ID
    ID end;   // 结束点ID
    E w;      // 边权

    Edge(const ID& begin, const ID& end, const E& w) :
            begin(begin), end(end), w(w) {}
};

/**
     * @brief 节点
     */
struct Vertex : public Object<Vertex> {
private:
    using self = Vertex;

public:
    ID id;                // 节点ID
    V w;                  // 点权
    DynArray<Edge> edges; // 该节点对应的边。如果为有向图，则代表以这个节点为起点的边
    bool visited;         // 可以判断是否已被访问
    Vertex* prev;         // 该节点的前驱

    Vertex(const ID& id, const V& w) :
            id(id), w(w), edges(), visited(false), prev(nullptr) {}
};

/**
     * @brief 简单路径
     */
struct Path {
};

} // namespace my::graph

#endif // GRAPH_HELPER_HPP