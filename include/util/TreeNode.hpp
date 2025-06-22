/**
 * @brief 树节点
 * @author Ricky
 * @date 2025/2/25
 * @version 1.0
 */
#ifndef TREE_NODE_HPP
#define TREE_NODE_HPP

#include "DynArray.hpp"
#include "Exception.hpp"
#include "KeyValue.hpp"
#include "Function.hpp"

namespace my::util {

/**
 * @brief 树节点类型约束
 */
template <typename T>
concept TreeNodeType = requires(T a, const T& b, T&& c) {
    { T() }
    ->std::same_as<T>;
    { a = std::move(c) }
    ->std::same_as<T&>;
    { T(std::move(c)) }
    ->std::same_as<T>;
    {a.value_};
    {a.children_};
    {a.parent_};
};

template <typename T>
concept BiTreeNodeType = requires(T a, const T& b, T&& c) {
    { T() }
    ->std::same_as<T>;
    { a = std::move(c) }
    ->std::same_as<T&>;
    { T(std::move(c)) }
    ->std::same_as<T>;
    {a.value_};
    {a.lchild_};
    {a.rchild_};
    {a.parent_};
};

template <typename T>
class TreeNode : public Object<TreeNode<T>> {
public:
    using value_t = T;
    using Self = TreeNode<value_t>;
    using Callback = Consumer<const value_t&>;

    value_t value_;            // 节点值
    Self* parent_;             // 指向父节点的指针，定义根节点的父指针指向自身
    DynArray<Self*> children_; // 指向子节点的指针集合

    explicit TreeNode(const value_t& value = value_t{}, Self* parent = nullptr) :
            value_(value), parent_(parent) {
        if (parent == nullptr) {
            parent_ = this; // 根节点指向自身
        }
    }

    TreeNode(const Self&) = delete;
    Self& operator=(const Self&) = delete;

    TreeNode(Self&& other) noexcept :
            value_(std::move(other.value_)), parent_(other.parent_), children_(std::move(other.children_)) {
        other.parent_ = nullptr;
    }

    Self& operator=(Self&& other) noexcept {
        if (this == &other) return *this;

        this->value_ = std::move(other.value_);
        this->parent_ = other.parent_;
        this->children_ = std::move(other.children_);
        return *this;
    }

    /**
     * @brief 遍历本节点之下的所有子节点，按照先序遍历排列
     */
    void for_each(Callback callback) const {
        callback(this->value_);
        for (const auto& child : children_) {
            child->for_each(callback);
        }
    }

    /**
     * @brief 遍历本节点之下的所有子节点，按照后序遍历排列
     */
    void for_each_rev(Callback callback) const {
        for (const auto& child : children_) {
            child->for_each_rev(callback);
        }
        callback(this->value_);
    }

    /**
     * @brief 遍历本节点以上的所有祖先节点
     */
    void for_each_parent(Callback callback) const {
        const Self* p = this;
        while (p != nullptr && p != p->parent_) {
            callback(p->value_);
            p = p->parent_;
        }
    }

    [[nodiscard]] CString __str__() const {
        std::stringstream stream;
        this->print(stream, "");
        return CString{stream.str()};
    }

private:
    void print(std::stringstream& stream, const CString& prefix) const {
        stream << prefix.data() << "+-- " << value_ << '\n';
        for (const auto& child : children_) {
            child->print(stream, prefix + "|   ");
        }
    }
};

/**
 * @brief 二叉树节点
 */
template <typename T>
class BiTreeNode : public Object<BiTreeNode<T>> {
public:
    using value_t = T;
    using Self = BiTreeNode<value_t>;
    using Callback = Consumer<const value_t&>;

    value_t value_; // 值
    Self* lchild_;  // 指向左孩子的指针
    Self* rchild_;  // 指向右孩子的指针
    Self* parent_;  // 指向父节点的指针，定义根节点的父指针指向自身

    explicit BiTreeNode(value_t value = value_t{}, Self* parent = nullptr) :
            value_(value), lchild_(nullptr), rchild_(nullptr), parent_(parent) {
        if (parent == nullptr) {
            parent_ = this; // 根节点指向自身
        }
    }

    BiTreeNode(const Self&) = delete;
    Self& operator=(const Self&) = delete;

    BiTreeNode(Self&& other) noexcept :
            value_(std::move(other.value)), lchild_(other.lchild_), rchild_(other.rchild_), parent_(other.parent_) {
        other.lchild_ = other.rchild_ = other.parent_ = nullptr;
    }

    Self& operator=(Self&& other) noexcept {
        if (this == &other) return *this;

        this->value_ = std::move(other.value_);
        this->lchild_ = other.lchild_;
        this->rchild_ = other.rchild_;
        this->parent_ = other.parent_;
        other.lchild_ = other.rchild_ = other.parent_ = nullptr;
        return *this;
    }

    /**
     * @brief 遍历本节点之下的所有子节点，按照先序遍历排列
     */
    void for_each(Callback callback) const {
        callback(this->value_);
        if (lchild_) lchild_->for_each(callback);
        if (rchild_) rchild_->for_each(callback);
    }

    /**
     * @brief 遍历本节点之下的所有子节点，按照后序遍历排列
     */
    void for_each_rev(Callback callback) const {
        if (lchild_) lchild_->for_each_rev(callback);
        if (rchild_) rchild_->for_each_rev(callback);
        callback(this->value_);
    }

    /**
     * @brief 遍历本节点以上的所有祖先节点
     */
    void for_each_parent(Callback callback) const {
        const Self* p = this;
        while (p != nullptr && p != p->parent_) {
            callback(p->value_);
            p = p->parent_;
        }
    }

    [[nodiscard]] CString __str__() const {
        std::stringstream stream;
        this->print(stream, "");
        return CString{stream.str()};
    }

private:
    void print(std::stringstream& stream, const CString& prefix) const {
        stream << prefix.data() << "+-- " << value_ << '\n';
        if (lchild_) lchild_->print(stream, prefix + "|   ");
        if (rchild_) rchild_->print(stream, prefix + "|   ");
    }
};

/**
 * @brief 红黑树节点颜色
 */
enum Color {
    BLACK, // 黑 = 0
    RED,   // 红 = 1
};

/**
 * @brief 红黑树节点，存储键值对
 */
template <Sortable K, typename V>
class RBTreeNode : public Object<RBTreeNode<K, V>> {
public:
    using key_t = K;
    using value_t = V;
    using Self = RBTreeNode<key_t, value_t>;
    using Callback = Consumer<const KeyValueView<key_t, value_t>&>;

    key_t key_;     // 键
    value_t value_; // 值
    Color color_;   // 颜色
    Self* lchild_;  // 指向左孩子的指针
    Self* rchild_;  // 指向右孩子的指针
    Self* parent_;  // 指向父节点的指针，定义根节点的父指针指向NIL

    explicit RBTreeNode(const key_t& key = key_t{}, const value_t& value = value_t{}, Color color = RED,
                        Self* lchild = nullptr, Self* rchild = nullptr, Self* parent = nullptr) :
            key_(key), value_(value), color_(color), lchild_(lchild), rchild_(rchild), parent_(parent) {}

    explicit RBTreeNode(key_t&& key, value_t&& value, Color color = RED) :
            key_(std::move(key)), value_(std::move(value)), color_(color), lchild_(nullptr), rchild_(nullptr), parent_(nullptr) {}

    RBTreeNode(const Self&) = delete;
    Self& operator=(const Self&) = delete;

    RBTreeNode(Self&& other) noexcept :
            color_(other.color_),
            key_(std::move(other.key_)),
            value_(std::move(other.value)),
            lchild_(other.lchild_),
            rchild_(other.rchild_),
            parent_(other.parent_) {
        other.lchild_ = other.rchild_ = other.parent_ = nullptr;
    }

    Self& operator=(Self&& other) noexcept {
        if (this == &other) return *this;

        this->color_ = other.color_;
        this->key_ = std::move(other.key_);
        this->value_ = std::move(other.value_);
        this->lchild_ = other.lchild_;
        this->rchild_ = other.rchild_;
        this->parent_ = other.parent_;
        other.lchild_ = other.rchild_ = other.parent_ = nullptr;
        return *this;
    }

    [[nodiscard]] cmp_t __cmp__(const Self& other) const {
        if constexpr (Comparable<key_t>) {
            return this->key_.__cmp__(other.key_);
        } else if constexpr (Subtractble<key_t>) {
            return this->key_ - other.key_;
        } else {
            type_exception("key type[{}] is not sortable", SRC_LOC, dtype(key_t));
        }
    }

    [[nodiscard]] CString __str__() const {
        std::stringstream stream;
        stream << (color_ == RED ? io::Color::RED : "") << '(' << key_ << ',' << value_ << ')' << io::Color::CLOSE << '\n';
        return CString{stream.str()};
    }
};

} // namespace my::util

#endif // TREE_NODE_HPP
