/**
 * @brief 基于B-Tree的有序Map
 * @author Ricky
 * @date 2025/7/12
 * @version 1.0
 */
#ifndef BTREE_MAP_HPP
#define BTREE_MAP_HPP

#include "key_value.hpp"

namespace my::util {

/**
 * @brief B-Tree节点约束
 */
template <typename T>
concept BTreeNodeType = requires(T a, const T& b, T&& c) {
    typename T::key_t;
    typename T::value_t;
    { T() } -> std::same_as<T>;
    { a = std::move(c) } -> std::same_as<T&>;
    { T(std::move(c)) } -> std::same_as<T>;
    { a.is_leaf };
    { a.key_cnt };
    { a.keys };
    { a.values };
    { a.subs };
    { a.p };
};

/**
 * @brief B-Tree节点
 * @note 值存在节点内，提高缓存局部性
 * @tparam K 键类型
 * @tparam V 值类型
 * @tparam N 阶数（每个节点最多存放 N 个键）
 */
template <Sortable K, typename V, usize N = 32>
struct BTreeNode : Object<BTreeNode<K, V, N>> {
    using key_t = K;
    using value_t = V;
    using Self = BTreeNode<K, V, N>;

    bool is_leaf;      // 是否为叶节点
    u16 key_cnt;       // 当前键数量
    key_t keys[N];     // 紧凑键数组
    value_t values[N]; // 值数组
    Self* subs[N + 1]; // 子节点数组
    Self* p;           // 父节点

    explicit BTreeNode(const bool is_leaf = true) :
            is_leaf(is_leaf), key_cnt(0), p(nullptr) {
        std::fill(std::begin(subs), std::end(subs), nullptr);
    }
};

/**
 * @class BTree
 * @brief B-Tree
 *
 */
template <BTreeNodeType Node, typename Comp = std::less<typename Node::key_t>, typename Alloc = mem::Allocator<Node>>
class BTree : public Object<BTree<Node, Comp, Alloc>> {
public:
    using Self = BTree<Node, Comp, Alloc>;
    using key_t = typename Node::key_t;
    using value_t = typename Node::value_t;
    using Callback = std::function<void(const key_t&, const value_t&)>;

    explicit BTree(Comp comp = Comp{}) :
            comp_(comp), len_(0) {
        this->root_ = alloc_.create(true);
    }

    ~BTree() {
        // TODO
    }

    /**
     * @brief 获取元素数量
     */
    usize len() const noexcept {
        return len_;
    }

    /**
     * @brief 判断是否为空
     */
    bool empty() const noexcept {
        return len_ == 0;
    }

    // TODO

private:
    Alloc alloc_{}; // 内存分配器
    Comp comp_;     // 比较函数
    usize len_;     // 节点个数
    Node* root_;    // 指向根节点的指针
};

/**
 * @brief 对外别名
 * @note 1. 由于现代存储器存在高速缓存，内存局部性高的数据结构性能远远高于内存局部性低的数据结构，而B树的内存局部性高于传统BST
 *       2. 由于作为有序map全部存储在内存中，B树相比于B+树内存占用更小，B树iter的性能与B+树差距微乎其微
 */
template <Sortable K, typename V, typename Comp = std::less<K>, typename Alloc = mem::Allocator<BTreeNode<K, V>>>
using BTreeMap = BTree<BTreeNode<K, V>, Comp, Alloc>;

} // namespace my::util

#endif // BTREE_MAP_HPP