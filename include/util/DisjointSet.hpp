/**
 * @brief 并查集
 * @author Ricky
 * @date 2025/1/23
 * @version 1.0
 */
#ifndef DISJOINT_SET_HPP
#define DISJOINT_SET_HPP

#include "Dict.hpp"

namespace my::util {

template <typename T>
class DisjointSet : public Object<DisjointSet<T>> {
    /**
     * @brief 并查集节点
     */
    struct Node : public Object<Node> {
        T value;      // 值
        i32 rank;     // 秩
        Node* parent; // 父节点

        Node(const T& value) :
                value(value), rank(1), parent(this) {}

        [[nodiscard]] CString __str__() const {
            return CString{std::format("([{}] {} {})", value, rank, parent ? parent->value : "null")};
        }
    };

public:
    using value_t = T;
    using Self = DisjointSet<value_t>;

    DisjointSet(const Vec<value_t>& arr) :
            nodes_(arr.size()) {
        for (auto&& item : arr) {
            nodes_.insert(item, new Node{item});
        }
    }

    DisjointSet(std::initializer_list<value_t>&& init_list) :
            DisjointSet(Vec<value_t>(std::move(init_list))) {}

    /**
     * @brief 添加元素
     */
    void add(const value_t& elem) {
        if (!nodes_.contains(elem)) {
            nodes_.insert(elem, new Node{elem});
        }
    }

    /**
     * @brief 查询组长
     * @return 返回该元素对应的组长，可能返回本身
     */
    value_t find(const value_t& elem) const {
        auto* cur = nodes_.get(elem);
        while (cur != cur->parent) {
            cur->parent = cur->parent->parent;
            cur = cur->parent;
        }
        return cur->value;
    }

    /**
     * @brief 判断两个元素是否属于同一组下
     * @return true=是 false=否
     */
    bool same_group(const value_t& elem1, const value_t& elem2) {
        return find(elem1) == find(elem2);
    }

    /**
     * @brief 合并，将两个元素按秩合并到一个组
     */
    void merge(const value_t& elem1, const value_t& elem2) {
        auto t1 = find(elem1);
        auto t2 = find(elem2);
        if (t1 == t2) return;
        auto* n1 = nodes_.get(t1);
        auto* n2 = nodes_.get(t2);
        if (n1->rank > n2->rank) {
            n2->parent = n1;
            n1->rank += n2->rank;
        } else {
            n1->parent = n2;
            n2->rank += n1->rank;
        }
    }

    /**
     * @brief 获取秩
     * @return 秩，若节点不存在返回 -1
     */
    i32 rank(const value_t& elem) {
        auto* n = nodes_.get(elem);
        if (n == nullptr) {
            return -1;
        }
        return n->rank;
    }

    [[nodiscard]] CString __str__() const {
        Dict<value_t, Vec<value_t>> sets;
        for (const auto& elem : nodes_.keys()) {
            sets[find(elem)].append(elem);
        }
        return sets.__str__();
    }

private:
    Dict<value_t, Node*> nodes_; // 节点集 元素->元素对应的节点
};

} // namespace my::util

#endif // DISJOINT_SET_HPP