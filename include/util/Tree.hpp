/**
 * @brief 树，用于表示层次结构
 * @author Ricky
 * @date 2025/2/25
 * @version 1.0
 */
#ifndef TREE_HPP
#define TREE_HPP

#include "TreeNode.hpp"
#include "Creator.hpp"
#include "Queue.hpp"

namespace my::util {

template <TreeNodeType Node, typename C = Creator<Node>>
class TreeImpl : public Object<TreeImpl<Node, C>> {
public:
    using Self = TreeImpl<Node, C>;
    using value_t = typename Node::value_t;
    using Callback = typename Node::Callback;

    TreeImpl() :
            size_(0), root_(nullptr) {}

    ~TreeImpl() {
        clear();
        root_ = nullptr;
    }

    Node* root() {
        return root_;
    }

    Node* root() const {
        return root_;
    }

    void clear() {
        destroy_node(root_);
        size_ = 0;
    }

    template <typename... Args>
    Node* set_root(Args&&... args) {
        root_ = creator_(std::forward<Args>(args)...);
        root_->parent_ = root_;
        size_ = 1;
        return root_;
    }

    template <typename... Args>
    Node* add_child(Node* parent, Args&&... args) {
        Node* child = creator_(std::forward<Args>(args)...);
        child->parent_ = parent;
        parent->children_.append(std::move(child));
        ++size_;
        return child;
    }

    void remove_child(Node* parent, isize idx = -1) {
        if (parent == nullptr || parent->children_.empty()) return;
        parent->children_.pop(idx);
        --size_;
    }

    /**
     * @brief 深度优先遍历
     * @param callback 回调函数，用于处理每个节点的值
     */
    void dfs(Callback callback) const {
        root_->for_each(callback);
    }

    /**
     * @brief 广度优先遍历
     * @param callback 回调函数，用于处理每个节点的值
     */
    void bfs(Callback callback) const {
        if (!root_) return;

        Queue<const Node*> q;
        q.push(root_);

        while (!q.empty()) {
            const auto* curr = q.front();
            q.pop();
            callback(curr->value_);
            for (const auto* child : curr->children_) {
                q.push(child);
            }
        }
    }

    [[nodiscard]] CString __str__() const {
        if (root_ == nullptr) {
            return "(empty tree)"_cs;
        }
        return root_->__str__();
    }

private:
    void destroy_node(Node* root) {
        if (root == nullptr) return;
        for (auto* child : root->children_) {
            destroy_node(child);
        }
        my_destroy(root);
    }

private:
    usize size_; // 节点个数
    Node* root_; // 根节点
    C creator_;  // 节点创建管理器
};

template <typename T>
using Tree = TreeImpl<TreeNode<T>, Creator<TreeNode<T>>>;

} // namespace my::util

#endif // TREE_HPP