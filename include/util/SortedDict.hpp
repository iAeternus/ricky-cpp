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
    using self = RBTree<Node, C>;
    using key_t = typename Node::key_t;
    using value_t = typename Node::value_t;

    // TODO

private:
    isize size_; // 节点个数
    Node* root_; // 指向根节点的指针
    C creator_;  // 节点创建管理器
};

/**
 * @brief 有序字典
 */
template <Comparable K, typename V>
using SortedDict = RBTree<RBTreeNode<K, V>, Creator<RBTreeNode<K, V>>>;

} // namespace my::util

#endif // SORTED_DICT_HPP
