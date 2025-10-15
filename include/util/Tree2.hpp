/**
 * @brief 树，用于表示层次结构
 * @author Ricky
 * @date 2025/9/13
 * @version 1.0
 */

#ifndef TREE2_HPP
#define TREE2_HPP

#include "Object.hpp"

namespace my::tmp {

/**
 * @brief 二叉树节点约束
 */
template <typename T>
concept BiTreeNodeType = requires(T a, const T& b, T&& c) {
    typename T::value_t;
    { T() } -> std::same_as<T>;
    { a = std::move(c) } -> std::same_as<T&>;
    { T(std::move(c)) } -> std::same_as<T>;
    { a.val };
    { a.lch };
    { a.rch };
    { a.p };
};

/**
 * @class TreeNode
 * @brief 二叉树节点
 * @tparam T 值类型
 */
template <typename T>
struct BiTreeNode : Object<BiTreeNode<T>> {
    using value_t = T;
    using Self = BiTreeNode<value_t>;

    value_t val; // 值
    Self* lch;   // 左儿子
    Self* rch;   // 右儿子
    Self* p;     // 父节点

    template <typename _T>
    explicit BiTreeNode(_T&& val, Self* lch = nullptr, Self* rch = nullptr, Self* p = nullptr) :
            val(std::forward<_T>(val)), lch(lch), rch(rch), p(p) {}

    BiTreeNode(const Self&) = delete;
    Self& operator=(const Self&) = delete;

    BiTreeNode(Self&& other) noexcept :
            val(std::move(other.val)), lch(other.lch), rch(other.rch), p(other.p) {
        other.lch = other.rch = other.p = nullptr;
    }

    Self& operator=(Self&& other) noexcept {
        if (this == &other) return *this;

        this->val = std::move(other.val);
        this->lch = other.lch;
        this->rch = other.rch;
        this->p = other.p;
        other.lch = other.rch = other.p = nullptr;
        return *this;
    }

    [[nodiscard]] CString __str__() const {
        return cstr(val);
    }
};

/**
 * @class TreeImpl
 * @brief 树，使用长子兄弟表示法
 * @tparam Node 节点类型
 * @tparam Alloc 内存分配器类型
 */
template <BiTreeNodeType Node, typename Alloc = Allocator<Node>>
class TreeImpl : public Object<TreeImpl<Node, Alloc>> {
public:
    using Self = TreeImpl<Node, Alloc>;
    using value_t = typename Node::value_t;
    using Callback = Consumer<const value_t&>;

    TreeImpl() :
            root_(nullptr), size_(0) {}

private:
    Alloc alloc_{}; // 内存分配器
    Node* root_;    // 根节点
    usize size_;    // 节点数
};

/**
 * @brief 树
 */
template <typename T>
using Tree = TreeImpl<BiTreeNode<T>>;

} // namespace my::tmp

#endif // TREE2_HPP
