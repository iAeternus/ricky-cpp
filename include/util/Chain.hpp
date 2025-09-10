/**
 * @brief 链容器
 * @author Ricky
 * @date 2024/12/14
 * @version 1.0
 */
#ifndef CHAIN_HPP
#define CHAIN_HPP

#include "ChainNode.hpp"
#include "Array.hpp"

namespace my::util {

template <ChainNodeType Node>
class ChainIterator;

/**
 * @brief 单向链
 */
template <ChainNodeType Node, typename Alloc = Allocator<Node>>
class Chain : public Object<Chain<Node, Alloc>> {
public:
    using Self = Chain<Node, Alloc>;
    using Super = Object<Self>;
    using value_t = typename Node::value_t;
    friend class ChainIterator<Node>;

    /**
     * @brief 默认构造函数
     * 初始化一个空单向链
     */
    Chain() :
            head_(nullptr), tail_(nullptr), size_(0) {}

    /**
     * @brief 析构函数
     */
    virtual ~Chain() {
        clear();
    }

    /**
     * @brief 获取链的大小
     * @return 返回链的大小
     */
    usize size() const {
        return size_;
    }

    /**
     * @brief 检查链是否为空
     * @return 如果链为空，返回 true；否则返回 false
     */
    bool empty() const {
        return size_ == 0;
    }

    /**
     * @brief 判断节点是否存在
     */
    bool contains(const Node& node) const {
        for (const auto& cur : *this) {
            if (cur == node.value_) {
                return true;
            }
        }
        return false;
    }

    /**
     * @brief 原位构造并在链尾追加一个新节点
     * @param args 用于初始化新节点的参数
     */
    template <typename... Args>
    void append(Args&&... args) {
        auto* new_node = alloc_.create(std::forward<Args>(args)...);

        if (size_ == 0) {
            head_ = new_node;
        } else {
            tail_->next_ = new_node;
        }
        tail_ = new_node;
        ++size_;
    }

    /**
     * @brief 清空单向链
     */
    void clear() {
        auto* cur = head_;
        Node* next = nullptr;
        while (cur) {
            next = cur->next_;
            alloc_.destruct(cur);
            cur = next;
        }
        head_ = tail_ = nullptr;
        size_ = 0;
    }

    /**
     * @brief 下标访问
     * @param index 索引
     */
    value_t& operator[](usize index) {
        index = neg_index(index, size_);

        auto* cur = head_;
        while (index--) {
            cur = cur->next_;
        }
        return cur->value_;
    }

    /**
     * @brief 下标访问（常量版本）
     * @param index 索引
     */
    const value_t& operator[](usize index) const {
        index = neg_index(index, size_);

        auto* cur = head_;
        while (index--) {
            cur = cur->next_;
        }
        return cur->value_;
    }

    /**
     * @brief 转换为Array，拷贝
     */
    Array<value_t> to_array() const {
        Array<value_t> arr(size_);
        usize pos = 0;
        for (const auto& cur : *this) {
            arr[pos++] = cur;
        }
        return arr;
    }

    /**
     * @brief 转换为Array，移动
     */
    Array<value_t> to_array() {
        Array<value_t> arr(size_);
        usize pos = 0;
        for (auto&& cur : *this) {
            arr[pos++] = std::forward<value_t>(cur);
        }
        clear();
        return arr;
    }

    [[nodiscard]] CString __str__() const {
        std::stringstream stream;
        stream << "<Chain> [";
        bool first = true;
        for (const auto& cur : *this) {
            if (first) {
                first = false;
            } else {
                stream << "->";
            }
            stream << cur;
        }
        stream << ']';
        return CString{stream.str()};
    }

    using iterator = ChainIterator<Node>;

    iterator begin() const {
        return iterator{head_};
    }

    iterator end() const {
        if (!tail_) {
            return iterator{nullptr};
        }
        return iterator{tail_->next_};
    }

protected:
    Alloc alloc_{};
    Node *head_, *tail_;
    usize size_;
};

/**
 * @brief 链迭代器
 */
template <ChainNodeType Node>
class ChainIterator : public Object<ChainIterator<Node>> {
    using Self = ChainIterator<Node>;

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = typename Node::value_t;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;

    ChainIterator(Node* node = nullptr) :
            current_(node) {}

    ChainIterator(const Self& other) :
            current_(other.current_) {}

    reference operator*() {
        return current_->value_;
    }

    const_reference operator*() const {
        return current_->value_;
    }

    pointer operator->() {
        return &current_->value_;
    }

    const_pointer operator->() const {
        return &current_->value_;
    }

    Self& operator++() {
        current_ = current_->next_;
        return *this;
    }

    Self operator++(i32) {
        Self tmp{current_};
        ++tmp;
        return tmp;
    }

    [[nodiscard]] cmp_t __cmp__(const Self& other) const {
        return this->current_ - other.current_;
    }

    bool operator==(const Self& other) const {
        return !this->__cmp__(other);
    }

    bool operator!=(const Self& other) const {
        return this->__cmp__(other);
    }

protected:
    Node* current_;
};

/**
 * @brief 单向链的别名
 * @tparam T 节点值类型
 * @tparam Alloc 内存分配器类型
 */
template <typename T, typename Alloc = Allocator<ChainNode<T>>>
using ChainList = Chain<ChainNode<T>, Alloc>;

} // namespace my::util

#endif // CHAIN_HPP