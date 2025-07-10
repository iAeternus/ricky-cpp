/**
 * @brief 树，用于表示层次结构
 * @author Ricky
 * @date 2025/2/25
 * @version 1.0
 */
#ifndef TREE_HPP
#define TREE_HPP

#include "Allocator.hpp"
#include "TreeNode.hpp"
#include "Queue.hpp"

namespace my::util {

template <TreeNodeType Node, typename Alloc = Allocator<Node>>
class TreeImpl : public Object<TreeImpl<Node, Alloc>> {
public:
    using Self = TreeImpl<Node, Alloc>;
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
        clear(root_);
        size_ = 0;
    }

    template <typename... Args>
    Node* set_root(Args&&... args) {
        root_ = alloc_.create(std::forward<Args>(args)...);
        root_->parent_ = root_;
        size_ = 1;
        return root_;
    }

    template <typename... Args>
    Node* add_child(Node* parent, Args&&... args) {
        Node* child = alloc_.create(std::forward<Args>(args)...);
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
    void clear(Node* root) {
        if (root == nullptr) return;
        for (auto* child : root->children_) {
            clear(child);
        }
        alloc_.destroy(root);
        alloc_.deallocate(root, 1);
    }

private:
    Alloc alloc_{}; // 内存分配器
    usize size_;    // 节点个数
    Node* root_;    // 根节点
};

template <typename T>
using Tree = TreeImpl<TreeNode<T>, Allocator<TreeNode<T>>>;

} // namespace my::util

#endif // TREE_HPP