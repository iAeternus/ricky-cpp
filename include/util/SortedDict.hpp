/**
 * @brief 有序字典，红黑树实现
 * @author Ricky
 * @date 2025/1/13
 * @version 1.0
 */
#ifndef SORTED_DICT_HPP
#define SORTED_DICT_HPP

#include "Pair.hpp"
#include "TreeNode.hpp"
#include <initializer_list>
#include <utility>

namespace my::util {

/**
 * @brief 红黑树
 * @tparam Node 红黑树节点类型，须满足BiTreeNodeType约束
 * @tparam Comp 比较二元函数，定义第一个参数位于左子树
 * @tparam Alloc 内存分配器类型
 * @note 满足如下红黑性质
 * 1. 每个节点要么是红色的，要么是黑色的
 * 2. 根节点是黑色的
 * 3. 叶结点（虚拟的外部节点，即NIL）是黑色的
 * 4. 不存在两个相邻的红节点（红节点的父节点和子节点均是黑色的）
 * 5. 对每个节点，从该节点到任意一个叶结点的简单路径上，所含黑节点的数量相同
 */
template <BiTreeNodeType Node, typename Comp = std::less<typename Node::key_t>, typename Alloc = Allocator<Node>>
class RBTree : public Object<RBTree<Node, Comp, Alloc>> {
public:
    using Self = RBTree<Node, Comp, Alloc>;
    using key_t = typename Node::key_t;
    using value_t = typename Node::value_t;
    using Callback = std::function<void(const key_t&, const value_t&)>;

    /**
     * @brief 构造一棵空红黑树
     */
    RBTree(Comp comp = Comp{}) :
            comp_(comp), size_(0) {
        create_nil();
        this->root_ = nil_;
    }

    RBTree(std::initializer_list<Pair<key_t, key_t>>&& init_list) :
            comp_(Comp{}), size_(init_list.size()) {
        create_nil();
        for (auto&& [key, val] : init_list) {
            insert(std::move(key), std::move(val));
        }
    }

    // TODO 拷贝构造

    /**
     * @brief 移动构造
     */
    RBTree(Self&& other) noexcept :
            alloc_(std::move(other.alloc_)),
            comp_(std::move(other.comp_)),
            size_(other.size_),
            root_(other.root_),
            nil_(other.nil_) {
        other.size_ = 0;
        other.root_ = other.nil_ = nullptr;
    }

    // TODO 拷贝赋值

    /**
     * @brief 移动赋值
     */
    Self& operator=(Self&& other) noexcept {
        if (this == &other) return *this;

        clear();
        if (nil_) {
            alloc_.destroy(nil_);
            alloc_.deallocate(nil_, 1);
        }

        this->alloc_ = std::move(other.alloc_);
        this->comp_ = std::move(other.comp_);
        this->size_ = other.size_;
        this->root_ = other.root_;
        this->nil_ = other.nil_;

        other.size_ = 0;
        other.root_ = other.nil_ = nullptr;
        return *this;
    }

    /**
     * @brief 析构
     */
    ~RBTree() {
        clear();
        if (nil_) {
            alloc_.destroy(nil_);
            alloc_.deallocate(nil_, 1);
            root_ = nil_ = nullptr;
        }
    }

    /**
     * @brief 获取节点个数
     */
    usize size() const noexcept {
        return size_;
    }

    /**
     * @brief 判断是否为空
     * @return true=是 false=否
     */
    bool empty() const noexcept {
        return root_ == nil_;
    }

    /**
     * @brief 构造并插入节点
     * @note 时间复杂度 O(log n)
     * @param args 构造节点的参数
     */
    template <typename... Args>
    void insert(Args&&... args) {
        auto* z = alloc_.create(std::forward<Args>(args)...); // 新节点
        Node* y = nil_;
        Node* x = root_;
        while (x != nil_) {
            y = x;
            if (comp_(z->key_, x->key_)) {
                x = x->lch_;
            } else {
                x = x->rch_;
            }
        }
        z->p_ = y;
        if (y == nil_) {
            root_ = z;
        } else if (comp_(z->key_, y->key_)) {
            y->lch_ = z;
        } else {
            y->rch_ = z;
        }
        z->lch_ = z->rch_ = nil_;
        z->color_ = Color::RED;
        insert_fixup(z); // 调整
        ++size_;
    }

    /**
     * @brief 删除节点
     * @param key 键
     */
    void remove(const key_t& key);
    void remove(key_t&& key);

    /**
     * @brief 清空所有节点
     */
    void clear() {
        clear(root_);
        root_ = nil_;
        size_ = 0;
    }

    void for_each(Callback callback) const {
        inorder_for_each(root_, callback);
    }

    /**
     * @brief 获取树结构
     */
    [[nodiscard]] CString tree_struct() const {
        if (root_ == nil_) {
            return "(empty dict)"_cs;
        }
        std::stringstream stream;
        print_tree(root_, stream, "");
        return CString{stream.str()};
    }

    [[nodiscard]] CString __str__() const {
        std::stringstream stream;
        stream << '{';
        for_each([&](const auto& key, const auto& val) {
            if constexpr (is_same<key_t, CString, String, std::string>) {
                stream << '\"' << key << '\"';
            } else {
                stream << key;
            }
            stream << ':';
            if constexpr (is_same<value_t, CString, String, std::string>) {
                stream << '\"' << val << '\"';
            } else {
                stream << val;
            }
            stream << ',';
        });
        auto str = stream.str();
        if (str.size() > 1) {
            str.pop_back();
        }
        str.push_back('}');
        return CString{str};
    }

    /**
     * @brief 红黑树迭代器
     */
    class RBTreeIterator : public Object<RBTreeIterator> {
    public:
        using Self = RBTreeIterator;

        // TODO

    private:
        Node* curr; // 迭代器当前位置
    };

    using iterator = RBTreeIterator;
    using const_iterator = RBTreeIterator;

    /**
     * @brief 获取字典的起始迭代器
     * @return 返回起始迭代器
     */
    iterator begin() {
        // TODO
    }

    /**
     * @brief 获取字典的起始迭代器（常量版本）
     * @return 返回常量起始迭代器
     */
    const_iterator begin() const {
        // TODO
    }

    /**
     * @brief 获取字典的末尾迭代器
     * @return 返回末尾迭代器
     */
    iterator end() {
        // TODO
    }

    /**
     * @brief 获取字典的末尾迭代器（常量版本）
     * @return 返回常量末尾迭代器
     */
    const_iterator end() const {
        // TODO
    }

private:
    /**
     * @brief 创建哨兵节点
     */
    void create_nil() {
        nil_ = alloc_.allocate(1);
        alloc_.construct(nil_, key_t{}, value_t{}, Color::BLACK);
        nil_->lch_ = nil_->rch_ = nil_;
    }

    /**
     * @brief 清空所有节点，递归辅助函数
     */
    void clear(Node* root) {
        if (root == nil_) return;
        clear(root->lch_);
        clear(root->rch_);
        alloc_.destroy(root);
        alloc_.deallocate(root, 1);
    }

    /**
     * @brief 打印树结构，递归辅助函数
     */
    void print_tree(const Node* root, std::stringstream& stream, const CString& prefix) const {
        if (root == nil_) return;
        stream << prefix.data() << "+-- " << root->__str__().data();
        print_tree(root->lch_, stream, prefix + "|   ");
        print_tree(root->rch_, stream, prefix + "|   ");
    }

    /**
     * @brief 中序遍历
     */
    void inorder_for_each(Node* node, Callback callback) const {
        if (node == nil_) return;
        inorder_for_each(node->lch_, callback);
        callback(node->key_, node->val_);
        inorder_for_each(node->rch_, callback);
    }

    /**
     * @brief 左旋
     *     |              |
     *     y    l_rot     x
     *    / \   <====    / \
     *   x   c          a   y
     *  / \     ====>      / \
     * a   b    r_rot     b   c
     */
    void left_rotate(Node* x) {
        Node* y = x->rch_; // 设置y
        x->rch_ = y->lch_; // 将y的左子树变为x的右子树
        if (y->lch_ != nil_) {
            y->lch_->p_ = x;
        }
        y->p_ = x->p_;
        if (x->p_ == nil_) {
            root_ = y;
        } else if (x == x->p_->lch_) {
            x->p_->lch_ = y;
        } else {
            x->p_->rch_ = y;
        }
        y->lch_ = x; // 将x放在y的左边
        x->p_ = y;
    }

    /**
     * @brief 右旋
     *     |              |
     *     y    l_rot     x
     *    / \   <====    / \
     *   x   c          a   y
     *  / \     ====>      / \
     * a   b    r_rot     b   c
     */
    void right_rotate(Node* x) {
        Node* y = x->lch_; // 设置y
        x->lch_ = y->rch_; // 将y的右子树变为x的左子树
        if (y->rch_ != nil_) {
            y->rch_->p_ = x;
        }
        y->p_ = x->p_;
        if (x->p_ == nil_) {
            root_ = y;
        } else if (x == x->p_->rch_) {
            x->p_->rch_ = y;
        } else {
            x->p_->lch_ = y;
        }
        y->rch_ = x; // 将x放在y的右边
        x->p_ = y;
    }

    /**
     * @brief 插入节点后调整
     * @note 存在以下3种情况
     * Case 1. z的叔节点y是红色的：将父/叔/爷节点反色，当前节点指向爷节点，继续调整
     * Case 2. z的叔节点y是黑色的且z是一个右孩子：左旋，转换为 case 3
     * Case 3. z的叔节点y是黑色的且z是一个左孩子：将父/爷节点反色，对爷节点右旋
     */
    void insert_fixup(Node* z) {
        while (z->p_->color_ == Color::RED) {
            if (z->p_ == z->p_->p_->lch_) {
                Node* y = z->p_->p_->rch_; // 叔节点
                if (y->color_ == Color::RED) {
                    // Case 1: 叔节点为红色
                    z->p_->color_ = y->color_ = Color::BLACK;
                    z->p_->p_->color_ = Color::RED;
                    z = z->p_->p_;
                } else {
                    // Case 2/3: 叔节点为黑色
                    if (z == z->p_->rch_) {
                        // Case 2: z是右孩子
                        z = z->p_;
                        left_rotate(z);
                    }
                    // Case 3: z是左孩子
                    z->p_->color_ = Color::BLACK;
                    z->p_->p_->color_ = Color::RED;
                    right_rotate(z->p_->p_);
                }
            } else {
                // 对称情况
                Node* y = z->p_->p_->lch_;
                if (y->color_ == Color::RED) {
                    z->p_->color_ = Color::BLACK;
                    y->color_ = Color::BLACK;
                    z->p_->p_->color_ = Color::RED;
                    z = z->p_->p_;
                } else {
                    if (z == z->p_->lch_) {
                        z = z->p_;
                        right_rotate(z);
                    }
                    z->p_->color_ = Color::BLACK;
                    z->p_->p_->color_ = Color::RED;
                    left_rotate(z->p_->p_);
                }
            }
        }
        root_->color_ = Color::BLACK;
    }

    /**
     * @brief 获取以node为根的子树的最小元素
     */
    Node* find_min(Node* node) const {
        if (node == nil_) return nil_;
        while (node->lch_ != nil_) {
            node = node->lch_;
        }
        return node;
    }

    /**
     * @brief 获取以node为根的子树的最大元素
     */
    Node* find_max(Node* node) const {
        if (node == nil_) return nil_;
        while (node->rch_ != nil_) {
            node = node->rch_;
        }
        return node;
    }

private:
    Alloc alloc_{}; // 内存分配器
    Comp comp_;     // 比较函数
    usize size_;    // 节点个数
    Node* root_;    // 指向根节点的指针
    Node* nil_;     // 哨兵：所有叶结点的孩子且是根节点的父亲
};

/**
 * @brief 有序字典
 */
template <Sortable K, typename V, typename Comp = std::less<K>>
using SortedDict = RBTree<RBTreeNode<K, V>, Comp, Allocator<RBTreeNode<K, V>>>;

} // namespace my::util

#endif // SORTED_DICT_HPP
