/**
 * @brief 图算法
 * @author Ricky
 * @date 2025/1/23
 * @version 1.0
 */
#ifndef GRAPH_ALGORITHM_HPP
#define GRAPH_ALGORITHM_HPP

#include "DisjointSet.hpp"
#include "Matrix.hpp"
#include "Graph.hpp"
#include "Queue.hpp"
#include "PriorityQueue.hpp"

namespace my::graph {

// #define INF F64_MAX

/**
 * @brief 邻接表转邻接矩阵
 * @note 会丢失点权信息，要求顶点id从0开始
 */
template <typename N = f64, typename E = f64, typename Idx = DefaultIdx>
auto adj2matrix = [](const auto& g, auto&& _) -> math::Matrix<E> {
    math::Matrix<E> m(g.node_cnt(), g.node_cnt(), TYPE_MAX(E)); // TODO
    g.for_each([&](const auto& vertex) {
        m[vertex.id][vertex.id] = 0;
        vertex.for_each([&](Idx v, E w) {
            m[vertex.id][v] = w;
        });
    });
    return m;
};

/**
 * @brief 判断图中是否存在EL路径
 * @note EL路径：包含所有边且长度为|E|的路径
 * @note 判断方式：度数为奇数的顶点个数为不大于2的偶数
 * @note 时间复杂度：O(|N|^2 * |E|)，若用邻接矩阵则为 O(|N|^2)
 */
template <typename N = f64, typename E = f64, typename Idx = DefaultIdx>
auto is_exist_el = [](const auto& g, auto&& _) -> bool {
    usize cnt = 0;
    g.for_each([&](const auto& vertex) {
        usize deg = vertex.out_deg() + g.in_deg(vertex.id);
        if (deg & 1) {
            ++cnt;
        }
    });
    return cnt == 0 || cnt == 2;
};

/**
 * @brief 广度优先搜索
 * @note 时间复杂度 O(|N| + |E|)
 * @note 空间复杂度 O(|N|)
 * @param s 源点
 * @param func 对每个访问到的节点执行的函数
 */
template <typename N = f64, typename E = f64, typename Idx = DefaultIdx>
auto bfs = [](const auto& g, auto&& args) {
    auto s = util::opt<Idx>(args, 0);
    auto func = util::opt<Consumer<const Node<N, E>&>>(args, 1);

    util::Queue<Node<N, E>> q;
    util::Vec<bool> vis(g.node_cnt(), false);

    auto node = g.get_node(s);
    q.push(node);
    func(node);
    vis[s] = true;
    while (!q.empty()) {
        auto node = q.front();
        q.pop();
        node.for_each([&](Idx v, E _) {
            if (vis[v]) return;
            auto adj = g.get_node(v);
            q.push(adj);
            vis[v] = true;
            func(adj);
        });
    }
};

/**
 * @brief 深度优先搜索
 * @note 时间复杂度 O(|N| + |E|)
 * @note 空间复杂度 O(|N|)
 * @param s 源点
 * @param func 对每个访问到的节点执行的函数
 */
template <typename N = f64, typename E = f64, typename Idx = DefaultIdx>
auto dfs = [](const auto& g, auto&& args) {
    auto s = util::opt<Idx>(args, 0);
    auto func = util::opt<Consumer<const Node<N, E>&>>(args, 1);
    util::Vec<bool> vis(g.node_cnt(), false);
    std::function<void(const Graph<N, E>&, Idx)> dfs_helper = [&](const Graph<>& g, Idx s) {
        auto node = g.get_node(s);
        func(node);
        vis[s] = true;
        node.for_each([&](Idx v, E _) {
            if (vis[v]) return;
            dfs_helper(g, v);
        });
    };

    dfs_helper(g, s);
};

/**
 * @brief 判断无向图是否为树
 * @note 无向图g为树，当且仅当g是无回路的连通图或者是n-1条边的连通图（n为g的顶点数）
 */
template <typename N = f64, typename E = f64, typename Idx = DefaultIdx>
auto is_tree = [](const auto& g, auto&& _) -> bool {
    auto n = g.node_cnt();
    usize node_cnt = 0, edge_cnt = 0;
    util::Vec<bool> vis(n, false);
    std::function<void(const Graph<N, E>&, Idx)> dfs_helper = [&](const Graph<>& g, Idx s) {
        auto node = g.get_node(s);
        node_cnt++;
        vis[node.id] = true;
        node.for_each([&](Idx v, E _) {
            if (vis[v]) return;
            edge_cnt++;
            dfs_helper(g, v);
        });
    };

    dfs_helper(g, 0); // TODO，随便选一个顶点开始
    return node_cnt == n && edge_cnt == n - 1;
};

/**
 * @brief 判断是否存在顶点s到t的路径
 * @note dfs实现
 * @param s 起点
 * @param t 终点
 * @return true=是 false=否
 */
template <typename N = f64, typename E = f64, typename Idx = DefaultIdx>
auto can_reach_dfs = [](const auto& g, auto&& args) -> bool {
    auto s = util::opt<Idx>(args, 0);
    auto t = util::opt<Idx>(args, 1);

    if (s == t) return true;

    bool is_reach = false;
    util::Vec<bool> vis(g.node_cnt(), false);
    std::function<void(Idx)> dfs_helper = [&](Idx curr) {
        if (is_reach) return;
        if (curr == t) {
            is_reach = true;
            return;
        }

        vis[curr] = true;
        auto node = g.get_node(curr);
        node.for_each([&](Idx v, E _) {
            if (vis[v] || is_reach) return;
            dfs_helper(v);
        });
    };

    dfs_helper(s);
    return is_reach;
};

/**
 * @brief 判断是否存在顶点s到t的路径
 * @note bfs实现
 * @param s 起点
 * @param t 终点
 * @return true=是 false=否
 */
template <typename N = f64, typename E = f64, typename Idx = DefaultIdx>
auto can_reach_bfs = [](const auto& g, auto&& args) -> bool {
    auto s = util::opt<Idx>(args, 0);
    auto t = util::opt<Idx>(args, 1);

    if (s == t) return true;

    util::Vec<bool> vis(g.node_cnt(), false);
    util::Queue<Idx> q;
    q.push(s);
    vis[s] = true;
    // 里面到达t外面循环也会退出
    while (!q.empty() && !vis[t]) {
        auto u = q.front();
        q.pop();
        g.get_node(u).for_each([&](Idx v, E _) {
            if (vis[v]) return;
            vis[v] = true;
            q.push(v);
            if (v == t) {
                return;
            }
        });
    }

    return vis[t];
};

/**
 * @brief 获取从s到t的所有简单路径
 * @note 使用回溯算法找出所有简单路径
 * @param s 起点
 * @param t 终点
 * @return 所有从s到t的简单路径
 */
template <typename N = f64, typename E = f64, typename Idx = DefaultIdx>
auto get_all_paths = [](const auto& g, auto&& args) -> util::Vec<SimplePath<Idx>> {
    auto s = util::opt<Idx>(args, 0);
    auto t = util::opt<Idx>(args, 1);

    util::Vec<SimplePath<Idx>> paths;

    // 如果起点和终点相同，返回一个只包含该点的路径
    if (s == t) {
        SimplePath<Idx> path;
        path.append_node(s);
        paths.append(path);
        return paths;
    }

    SimplePath<Idx> curr_path;
    util::Vec<bool> vis(g.node_cnt(), false);

    curr_path.append_node(s);
    vis[s] = true;
    std::function<void(Idx)> dfs_helper = [&](Idx curr) {
        if (curr == t) {
            paths.append(curr_path);
            return;
        }

        g.get_node(curr).for_each([&](Idx v, E _) {
            if (vis[v]) return;
            curr_path.append_node(v);
            vis[v] = true;
            dfs_helper(v);
            curr_path.pop_node();
            vis[v] = false;
        });
    };

    dfs_helper(s);
    return paths;
};

/**
 * @brief Prim算法求最小生成树
 * @note 时间复杂度 O(|N|^2)，不依赖|E|
 * @note 适合稠密图的最小生成树求解。假设图是连通的
 */
template <typename N = f64, typename E = f64, typename Idx = DefaultIdx>
auto prim = [](const auto& g, auto&& _) -> Tree<N, E, Idx> {
    Tree<N, E, Idx> t{false};
    auto n = g.node_cnt();
    if (n == 0) return t;

    util::Vec<bool> vis(n, false);
    util::Vec<E> dis(n, TYPE_MAX(E)); // 到树的最小距离
    util::Vec<Idx> fa(n, npos);       // MST父节点ID

    dis[0] = 0; // TODO 任选一个节点开始
    for (Idx i = 0; i < n; ++i) {
        // 1. 找到未访问节点中距离最小的节点
        Idx u = npos;
        E min_dis = TYPE_MAX(E);
        g.for_each([&](const auto& node) {
            if (!vis[node.id] && dis[node.id] < min_dis) {
                min_dis = dis[node.id];
                u = node.id;
            }
        });

        // 若没有找到有效节点，说明图不连通
        if (u == npos) break;

        // 2. 添加节点和边到MST
        vis[u] = true;
        t.add_node(u);
        if (fa[u] != npos) {
            t.add_edge(fa[u], u, dis[u]);
        }

        // 3. 更新邻接节点的距离
        g.get_node(u).for_each([&](Idx v, E w) {
            // 只更新未访问且距离更小的节点
            if (!vis[v] && w < dis[v]) {
                dis[v] = w;
                fa[v] = u;
            }
        });
    }

    return t;
};

/**
 * @brief Prim算法求最小生成树（优先队列优化版）
 * @note 时间复杂度 O(|N|log|E|)，适合稀疏图
 * @note 假设图是连通的
 */
template <typename N = f64, typename E = f64, typename Idx = DefaultIdx>
auto prim2 = [](const auto& g, auto&& _) -> Tree<N, E, Idx> {
    Tree<N, E, Idx> t{false};
    auto n = g.node_cnt();
    if (n == 0) return t;

    util::Vec<bool> vis(n, false);
    util::Vec<E> dis(n, TYPE_MAX(E)); // 到树的最小距离
    util::Vec<Idx> fa(n, npos);       // MST父节点ID

    // 优先队列：存储 (距离, 节点ID) ，最小距离优先
    using Elem = Pair<E, Idx>;
    auto cmp = [](const Elem& a, const Elem& b) {
        return a.first() < b.first(); // 最小堆
    };
    util::PriorityQueue<Elem, decltype(cmp)> pq;

    dis[0] = 0; // TODO 任选一个节点开始
    pq.push(0, 0);
    while (!pq.empty()) {
        // 1. 取出距离最小的未访问节点
        auto [min_dis, u] = pq.top();
        pq.pop();

        // 跳过已访问节点
        if (vis[u]) continue;

        // 2. 添加节点和边到MST
        vis[u] = true;
        t.add_node(u);
        if (fa[u] != npos) {
            t.add_edge(fa[u], u, min_dis);
        }

        // 3. 更新邻接节点的距离
        g.get_node(u).for_each([&](Idx v, E w) {
            // 只更新未访问且距离更小的节点
            if (!vis[v] && w < dis[v]) {
                dis[v] = w;
                fa[v] = u;
                pq.push(w, v); // 将更新后的节点加入队列
            }
        });
    }

    return t;
};

/**
 * @brief Kruskal 算法求解最小生成树
 * @note 时间复杂度 O(|E|log|E|)，不依赖|V|
 * @note 适合于边稀疏而顶点较多的图
 */
template <typename N = f64, typename E = f64, typename Idx = DefaultIdx>
auto kruskal = [](const auto& g, auto&& _) -> Tree<N, E, Idx> { // TODO 段错误
    Tree<N, E, Idx> t{false};
    auto n = g.node_cnt();
    if (n == 0) return t;

    // 1. 添加所有节点到生成树
    g.for_each([&](const auto& node) {
        t.add_node(node.id);
    });

    // 2. 初始化并查集和优先队列
    util::DisjointSet<Idx> ds{};
    g.for_each([&](const auto& node) {
        ds.add(node.id); // 将每个节点添加到并查集
    });

    // 定义边结构体和优先队列比较函数
    struct Edge {
        Idx u, v;
        E w;
        Edge(Idx u, Idx v, E w) :
                u(u), v(v), w(w) {}
    };

    auto cmp = [](const Edge& a, const Edge& b) {
        return a.w < b.w; // 最小堆：权重小的优先级高
    };

    util::PriorityQueue<Edge, decltype(cmp)> pq;

    // 3. 遍历所有边并加入优先队列（无向图每条边只添加一次）
    g.for_each([&](const auto& node) {
        node.for_each([&](Idx v, E w) {
            // 只添加 u < v 的边，避免重复处理无向边
            if (node.id < v) {
                pq.push(node.id, v, w);
            }
        });
    });

    // 4. 处理优先队列中的边
    usize edge_count = 0;
    while (!pq.empty() && edge_count < n - 1) {
        auto edge = pq.top();
        pq.pop();

        // 检查是否形成环路
        if (!ds.same_group(edge.u, edge.v)) {
            // 添加边到生成树
            t.add_edge(edge.u, edge.v, edge.w);
            // 合并两个连通分量
            ds.merge(edge.u, edge.v);
            edge_count++;
        }
    }

    return t;
};

/**
 * @brief Dijkstra 算法求解单源最短路径
 * @note 时间复杂度 O(|E|log|E|)，若不用优先队列优化，时间复杂度为 O(|V|^2)
 * @note 假设图中没有负权边
 * @param s 源点
 */
template <typename N = f64, typename E = f64, typename Idx = DefaultIdx>
auto dijkstra = [](const auto& g, auto&& args) -> util::Vec<E> {
    auto s = util::opt<Idx>(args, 0);

    usize n = g.node_cnt();
    util::Vec<E> dis(n, TYPE_MAX(E));
    dis[s] = E{};

    using Node = Pair<E, Idx>;
    util::PriorityQueue<Node, std::greater<>> pq;
    pq.push(E{}, s);

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        // 若当前距离大于已记录距离，跳过无效节点
        if (d != dis[u]) continue;

        g.get_node(u).for_each([&](Idx v, E w) {
            auto new_dis = d + w;
            // 松弛操作：发现更短路径时更新
            if (new_dis < dis[v]) {
                dis[v] = new_dis;
                pq.push(new_dis, v);
            }
        });
    }

    return dis;
};

/**
 * @brief 所有点对最短路径，floyd 算法
 * @note 时间复杂度 O(|N|^3)
 * @note 可适用于负权图，不适用于有负环的图
 */
template <typename N = f64, typename E = f64, typename Idx = DefaultIdx>
auto floyd = [](const auto& g, auto&& _) -> math::Matrix<E> {
    auto n = g.node_cnt();
    math::Matrix<E> dis(n, n, TYPE_MAX(E));
    for (usize i = 0; i < n; ++i) {
        dis[i][i] = E{};
    }

    g.for_each([&](const auto& node) {
        node.for_each([&](Idx v, E w) {
            dis[node.id][v] = w;
        });
    });

    for (usize k = 0; k < n; ++k) {
        for (usize u = 0; u < n; ++u) {
            if (dis[u][k] == TYPE_MAX(E)) continue;
            for (usize v = 0; v < n; ++v) {
                dis[u][v] = std::min(dis[u][v], dis[u][k] + dis[k][v]);
            }
        }
    }
    return dis;
};

// /**
//  * @brief 拓扑排序，生成其中一条拓扑序
//  * @note 时间复杂度 O(|N| + |E|)
//  */
// template <typename N = f64, typename E = f64, typename Idx = DefaultIdx>
// auto topological_sort_bfs = [](const auto& g, auto&& _) -> util::Vec<Idx> {
//     // TODO
// };

} // namespace my::graph

#endif // GRAPH_ALGORITHM_HPP