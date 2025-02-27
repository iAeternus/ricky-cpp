/**
 * @brief 树节点
 * @author Ricky
 * @date 2025/2/25
 * @version 1.0
 */
#ifndef TREE_NODE_HPP
#define TREE_NODE_HPP

#include "DynArray.hpp"
#include "KeyValue.hpp"

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
    using self = TreeNode<T>;
    using value_t = T;
    using Callback = std::function<void(const value_t&)>;

    value_t value_;            // 节点值
    self* parent_;             // 指向父节点的指针，定义根节点的父指针指向自身
    DynArray<self*> children_; // 指向子节点的指针集合

    explicit TreeNode(const value_t& value = value_t{}, self* parent = nullptr) :
            value_(value), parent_(parent) {
        if (parent == nullptr) {
            parent_ = this; // 根节点指向自身
        }
    }

    TreeNode(const self&) = delete;
    self& operator=(const self&) = delete;

    TreeNode(self&& other) noexcept :
            value_(std::move(other.value_)), parent_(other.parent_), children_(std::move(other.children_)) {
        other.parent_ = nullptr;
    }

    self& operator=(self&& other) noexcept {
        if (this == &other) return *this;

        this->value_ = std::move(other.value_);
        this->parent_ = other.parent_;
        this->children_ = std::move(other.children_);
        return *this;
    }

    /**
     * @brief 遍历本节点之下的所有子节点，按照先序遍历排列
     */
    void forEach(Callback callback) const {
        callback(this->value_);
        for (const auto& child : children_) {
            child->forEach(callback);
        }
    }

    /**
     * @brief 遍历本节点之下的所有子节点，按照后序遍历排列
     */
    void forEachRev(Callback callback) const {
        for (const auto& child : children_) {
            child->forEachRev(callback);
        }
        callback(this->value_);
    }

    /**
     * @brief 遍历本节点以上的所有祖先节点
     */
    void forEachParent(Callback callback) const {
        const self* p = this;
        while (p != nullptr && p != p->parent_) {
            callback(p->value_);
            p = p->parent_;
        }
    }

    CString __str__() const {
        std::stringstream stream;
        this->print(stream, "");
        return CString{stream.str()};
    }

private:
    void print(std::stringstream& stream, const CString& prefix) const {
        stream << prefix.data() << "+-- " << value_ << '\n';
        const auto childCount = children_.size();
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
    using self = BiTreeNode<T>;
    using value_t = T;
    using Callback = std::function<void(const value_t&)>;

    value_t value_; // 值
    self* lchild_;  // 指向左孩子的指针
    self* rchild_;  // 指向右孩子的指针
    self* parent_;  // 指向父节点的指针，定义根节点的父指针指向自身

    explicit BiTreeNode(value_t value = value_t{}, self* parent = nullptr) :
            value_(value), lchild_(nullptr), rchild_(nullptr), parent_(parent) {
        if (parent == nullptr) {
            parent_ = this; // 根节点指向自身
        }
    }

    BiTreeNode(const self&) = delete;
    self& operator=(const self&) = delete;

    BiTreeNode(self&& other) noexcept :
            value_(std::move(other.value)), lchild_(other.lchild_), rchild_(other.rchild_), parent_(other.parent_) {
        other.lchild_ = other.rchild_ = other.parent_ = nullptr;
    }

    self& operator=(self&& other) noexcept {
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
    void forEach(Callback callback) const {
        callback(this->value_);
        lchild_->forEach(callback);
        rchild_->forEach(callback);
    }

    /**
     * @brief 遍历本节点之下的所有子节点，按照后序遍历排列
     */
    void forEachRev(Callback callback) const {
        lchild_->forEachRev(callback);
        rchild_->forEachRev(callback);
        callback(this->value_);
    }

    /**
     * @brief 遍历本节点以上的所有祖先节点
     */
    void forEachParent(Callback callback) const {
        const self* p = this;
        while (p != nullptr && p != p->parent_) {
            callback(p->value_);
            p = p->parent_;
        }
    }

    CString __str__() const {
        std::stringstream stream;
        this->print(stream, "");
        return CString{stream.str()};
    }

private:
    void print(std::stringstream& stream, const CString& prefix) const {
        stream << prefix.data() << "+-- " << value_ << '\n';
        lchild_->print(stream, prefix + "|   ");
        rchild_->print(stream, prefix + "|   ");
    }
};

/**
 * @brief 红黑树节点颜色
 */
enum Color {
    RED,   // 红 = 0
    BLACK, // 黑 = 1
};

/**
 * @brief 红黑树节点，存储键值对
 */
template <Comparable K, typename V>
class RBTreeNode : public Object<RBTreeNode<K, V>> {
public:
    using self = RBTreeNode<K, V>;
    using key_t = K;
    using value_t = V;
    using Callback = std::function<void(const KeyValueView<key_t, value_t>&)>;

    Color color_;   //颜色
    key_t key_;     // 键
    value_t value_; // 值
    self* lchild_;  // 指向左孩子的指针
    self* rchild_;  // 指向右孩子的指针
    self* parent_;  // 指向父节点的指针，定义根节点的父指针指向自身

    explicit RBTreeNode(const key_t& key = key_t{}, const value_t& value = value_t{}, self* parent = nullptr) :
            color_(BLACK), key_(key), value_(value), lchild_(nullptr), rchild_(nullptr), parent_(parent) {
        if (parent_ == nullptr) {
            parent_ = this;
        }
    }

    RBTreeNode(const self&) = delete;
    self& operator=(const self&) = delete;

    RBTreeNode(self&& other) noexcept :
            color_(other.color_),
            key_(std::move(other.key_)),
            value_(std::move(other.value)),
            lchild_(other.lchild_),
            rchild_(other.rchild_),
            parent_(other.parent_) {
        other.lchild_ = other.rchild_ = other.parent_ = nullptr;
    }

    self& operator=(self&& other) noexcept {
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

    /**
     * @brief 遍历本节点之下的所有子节点，按照先序遍历排列
     */
    void forEach(Callback callback) const {
        callback(this->value_);
        lchild_->forEach(callback);
        rchild_->forEach(callback);
    }

    /**
     * @brief 遍历本节点之下的所有子节点，按照后序遍历排列
     */
    void forEachRev(Callback callback) const {
        lchild_->forEachRev(callback);
        rchild_->forEachRev(callback);
        callback(this->value_);
    }

    /**
     * @brief 遍历本节点以上的所有祖先节点
     */
    void forEachParent(Callback callback) const {
        const self* p = this;
        while (p != nullptr && p != p->parent_) {
            callback(p->value_);
            p = p->parent_;
        }
    }

    CString __str__() const {
        std::stringstream stream;
        this->print(stream, "");
        return CString{stream.str()};
    }

private:
    void print(std::stringstream& stream, const CString& prefix) const {
        stream << prefix.data() << "+-- " << value_ << '\n';
        lchild_->print(stream, prefix + "|   ");
        rchild_->print(stream, prefix + "|   ");
    }
};

} // namespace my::util

#endif // TREE_NODE_HPP
