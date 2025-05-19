/**
 * @brief 有序字典，红黑树实现
 * @author Ricky
 * @date 2025/1/13
 * @version 1.0
 */
#ifndef SORTED_DICT_HPP
#define SORTED_DICT_HPP

#include "TreeNode.hpp"
#include "Creator.hpp"

namespace my::util {

/**
 * @brief 红黑树
 */
template <BiTreeNodeType Node, typename C = Creator<Node>>
class RBTree : public Object<RBTree<Node, C>> {
public:
    using Self = RBTree<Node, C>;
    using key_t = typename Node::key_t;
    using value_t = typename Node::value_t;

    RBTree() :
            size_(0), root_(NIL) {
        NIL->lchild_ = NIL->rchild_ = root_;
    }

    ~RBTree() {
        clear();
        my_destroy(NIL);
        my_delloc(NIL);
        root_ = NIL = nullptr;
    }

    void clear() {
        destroy_node(root_);
        root_ = NIL;
        size_ = 0;
    }

    [[nodiscard]] CString __str__() const {
        if (root_ == NIL) {
            return "(empty dict)"_cs;
        }
        std::stringstream stream;
        print_node(root_, stream, "");
        return CString{stream.str()};
    }

private:
    usize size_; // 节点个数
    Node* root_; // 指向根节点的指针
    C creator_;  // 节点创建管理器

    // 哨兵：所有叶结点的孩子且是根节点的父亲
    static inline Node* NIL = new Node(key_t{}, value_t{}, BLACK);

private:
    static void destroy_node(Node* root) {
        if (root == NIL) return;
        destroy_node(root->lchild_);
        destroy_node(root->rchild_);
        my_destroy(root);
    }

    static void print_node(const Node* root, std::stringstream& stream, const CString& prefix) {
        if (root == NIL) return;
        stream << prefix.data() << "+-- " << root->__str__().data();
        print_node(root->lchild_, stream, prefix + "|   ");
        print_node(root->rchild_, stream, prefix + "|   ");
    }
};

/**
 * @brief 有序字典
 */
template <Sortable K, typename V>
using SortedDict = RBTree<RBTreeNode<K, V>, Creator<RBTreeNode<K, V>>>;

} // namespace my::util

#endif // SORTED_DICT_HPP
