/**
 * @brief 树，用于表示层次结构
 * @author Ricky
 * @date 2025/2/25
 * @version 1.0
 */
#ifndef TREE_HPP
#define TREE_HPP

#include "DynArray.hpp"
#include "Queue.hpp"

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
    {a.val_};
    {a.subs_};
    {a.p_};
};

/**
 * @brief 树节点
 */
template <typename T>
class TreeNode : public Object<TreeNode<T>> {
public:
    using value_t = T;
    using Self = TreeNode<value_t>;
    using Callback = Consumer<const value_t&>;

    value_t val_;          // 节点值
    Self* p_;              // 指向父节点的指针，定义根节点的父指针指向自身
    DynArray<Self*> subs_; // 指向子节点的指针集合

    explicit TreeNode(const value_t& value = value_t{}, Self* parent = nullptr) :
            val_(value), p_(parent) {
        if (parent == nullptr) {
            p_ = this; // 根节点指向自身
        }
    }

    TreeNode(const Self&) = delete;
    Self& operator=(const Self&) = delete;

    TreeNode(Self&& other) noexcept :
            val_(std::move(other.val_)), p_(other.p_), subs_(std::move(other.subs_)) {
        other.p_ = nullptr;
    }

    Self& operator=(Self&& other) noexcept {
        if (this == &other) return *this;

        this->val_ = std::move(other.val_);
        this->p_ = other.p_;
        this->subs_ = std::move(other.subs_);
        return *this;
    }

    /**
     * @brief 遍历本节点之下的所有子节点，按照先序遍历排列
     */
    void for_each(Callback callback) const {
        callback(this->val_);
        for (const auto& sub : subs_) {
            sub->for_each(callback);
        }
    }

    /**
     * @brief 遍历本节点之下的所有子节点，按照后序遍历排列
     */
    void for_each_rev(Callback callback) const {
        for (const auto& sub : subs_) {
            sub->for_each_rev(callback);
        }
        callback(this->val_);
    }

    /**
     * @brief 遍历本节点以上的所有祖先节点
     */
    void for_each_parent(Callback callback) const {
        const Self* p = this;
        while (p != nullptr && p != p->p_) {
            callback(p->val_);
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
        stream << prefix.data() << "+-- " << val_ << '\n';
        for (const auto& sub : subs_) {
            sub->print(stream, prefix + "|   ");
        }
    }
};

/**
 * @brief 树
 */
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
        root_->p_ = root_;
        size_ = 1;
        return root_;
    }

    template <typename... Args>
    Node* add_child(Node* parent, Args&&... args) {
        Node* sub = alloc_.create(std::forward<Args>(args)...);
        sub->p_ = parent;
        parent->subs_.append(std::move(sub));
        ++size_;
        return sub;
    }

    void remove_child(Node* parent, isize idx = -1) {
        if (parent == nullptr || parent->subs_.empty()) return;
        parent->subs_.pop(idx);
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
            callback(curr->val_);
            for (const auto* sub : curr->subs_) {
                q.push(sub);
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
        for (auto* sub : root->subs_) {
            clear(sub);
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