/**
 * @brief 树节点
 * @author Ricky
 * @date 2025/2/25
 * @version 1.0
 */
#ifndef TREE_NODE_HPP
#define TREE_NODE_HPP

#include "DynArray.hpp"

namespace my::util {

/**
 * @brief 树节点类型约束
 */
template <typename T>
concept TreeNodeType = requires(T a, const T& b, T&& c) {
    { T() }
    ->std::same_as<T>;
    // { a = b }
    // ->std::same_as<T&>;
    // { T(b) }
    // ->std::same_as<T>;
    { a = std::move(c) }
    ->std::same_as<T&>;
    { T(std::move(c)) }
    ->std::same_as<T>;
    {a.value_};
    {a.children_};
    {a.parent_};
};

template <typename T>
class TreeNode : public Object<TreeNode<T>> {
    using self = TreeNode<T>;

public:
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
    void print(std::stringstream& stream, const std::string& prefix) const {
        stream << prefix << "+-- " << value_ << '\n';
        const auto childCount = children_.size();
        for (const auto& child : children_) {
            child->print(stream, prefix + "|   ");
        }
    }
};

} // namespace my::util

#endif // TREE_NODE_HPP