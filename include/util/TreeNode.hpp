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
#include "Printer.hpp"

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
    typename T::key_t;
    typename T::value_t;

    { T() }
    ->std::same_as<T>;
    { a = std::move(c) }
    ->std::same_as<T&>;
    { T(std::move(c)) }
    ->std::same_as<T>;
    {a.key_};
    {a.lch_};
    {a.rch_};
    {a.p_};
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
    using key_t = T;
    using value_t = T;
    using Self = BiTreeNode<key_t>;
    using Callback = Consumer<const key_t&>;

    key_t key_; // 关键字
    Self* lch_; // 指向左孩子的指针
    Self* rch_; // 指向右孩子的指针
    Self* p_;   // 指向父节点的指针，定义根节点的父指针指向自身

    explicit BiTreeNode(key_t key = key_t{}, Self* parent = nullptr) :
            key_(key), lch_(nullptr), rch_(nullptr), p_(parent) {
        if (parent == nullptr) {
            p_ = this; // 根节点指向自身
        }
    }

    BiTreeNode(const Self&) = delete;
    Self& operator=(const Self&) = delete;

    BiTreeNode(Self&& other) noexcept :
            key_(std::move(other.key)), lch_(other.lch_), rch_(other.rch_), p_(other.p_) {
        other.lch_ = other.rch_ = other.p_ = nullptr;
    }

    Self& operator=(Self&& other) noexcept {
        if (this == &other) return *this;

        this->key_ = std::move(other.key_);
        this->lch_ = other.lch_;
        this->rch_ = other.rch_;
        this->p_ = other.p_;
        other.lch_ = other.rch_ = other.p_ = nullptr;
        return *this;
    }

    /**
     * @brief 遍历本节点之下的所有子节点，按照先序遍历排列
     */
    void for_each(Callback callback) const {
        callback(this->key_);
        if (lch_) lch_->for_each(callback);
        if (rch_) rch_->for_each(callback);
    }

    /**
     * @brief 遍历本节点之下的所有子节点，按照后序遍历排列
     */
    void for_each_rev(Callback callback) const {
        if (lch_) lch_->for_each_rev(callback);
        if (rch_) rch_->for_each_rev(callback);
        callback(this->key_);
    }

    /**
     * @brief 遍历本节点以上的所有祖先节点
     */
    void for_each_parent(Callback callback) const {
        const Self* p = this;
        while (p != nullptr && p != p->p_) {
            callback(p->key_);
            p = p->p_;
        }
    }

    [[nodiscard]] CString __str__() const {
        std::stringstream stream;
        this->print(stream, "");
        return CString{stream.str()};
    }

private:
    void print(std::stringstream& stream, const CString& prefix) const {
        stream << prefix.data() << "+-- " << key_ << '\n';
        if (lch_) lch_->print(stream, prefix + "|   ");
        if (rch_) rch_->print(stream, prefix + "|   ");
    }
};

/**
 * @brief 红黑树节点颜色
 */
enum class Color : bool {
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

    key_t key_;   // 键
    value_t val_; // 值
    Color color_; // 颜色
    Self* lch_;   // 指向左孩子的指针
    Self* rch_;   // 指向右孩子的指针
    Self* p_;     // 指向父节点的指针，定义根节点的父指针指向NIL

    explicit RBTreeNode(const key_t& key = key_t{}, const value_t& value = value_t{}, Color color = Color::RED,
                        Self* lchild = nullptr, Self* rchild = nullptr, Self* parent = nullptr) :
            key_(key), val_(value), color_(color), lch_(lchild), rch_(rchild), p_(parent) {}

    explicit RBTreeNode(key_t&& key, value_t&& value, Color color = Color::RED) :
            key_(std::move(key)), val_(std::move(value)), color_(color), lch_(nullptr), rch_(nullptr), p_(nullptr) {}

    RBTreeNode(const Self&) = delete;
    Self& operator=(const Self&) = delete;

    RBTreeNode(Self&& other) noexcept :
            color_(other.color_),
            key_(std::move(other.key_)),
            val_(std::move(other.value)),
            lch_(other.lch_),
            rch_(other.rch_),
            p_(other.p_) {
        other.lch_ = other.rch_ = other.p_ = nullptr;
    }

    Self& operator=(Self&& other) noexcept {
        if (this == &other) return *this;

        this->color_ = other.color_;
        this->key_ = std::move(other.key_);
        this->val_ = std::move(other.val_);
        this->lch_ = other.lch_;
        this->rch_ = other.rch_;
        this->p_ = other.p_;
        other.lch_ = other.rch_ = other.p_ = nullptr;
        return *this;
    }

    [[nodiscard]] cmp_t __cmp__(const Self& other) const {
        if constexpr (Comparable<key_t>) {
            return this->key_.__cmp__(other.key_);
        } else if constexpr (Subtractble<key_t>) {
            return this->key_ - other.key_;
        } else {
            throw type_exception("key type[{}] is not sortable", SRC_LOC, dtype(key_t));
        }
    }

    [[nodiscard]] CString __str__() const {
        std::stringstream stream;
        stream << (color_ == Color::RED ? io::Color::RED : "") << '(' << key_ << ',' << val_ << ')' << io::Color::CLOSE << '\n';
        return CString{stream.str()};
    }
};

} // namespace my::util

#endif // TREE_NODE_HPP
