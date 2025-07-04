/**
 * @brief 链容器
 * @author Ricky
 * @date 2024/12/14
 * @version 1.0
 */
#ifndef CHAIN_HPP
#define CHAIN_HPP

#include "Allocator.hpp"
#include "Creator.hpp"
#include "ChainNode.hpp"
#include "Array.hpp"

namespace my::util {

template <ChainNodeType Node>
class ChainIterator;

/**
 * @brief 链
 */
template <ChainNodeType Node, typename C = Creator<Node>, typename Alloc = Allocator<Node>>
class Chain : public Object<Chain<Node, C, Alloc>> {
public:
    using Self = Chain<Node, C, Alloc>;
    using Super = Object<Self>;
    using value_t = typename Node::value_t;
    friend class ChainIterator<Node>;

    Chain() :
            head_(nullptr), tail_(nullptr), size_(0) {}

    virtual ~Chain() {
        clear();
    }

    usize size() const {
        return size_;
    }

    bool empty() const {
        return size_ == 0;
    }

    bool contains(const Node& node) const {
        for (const auto& cur : *this) {
            if (cur == node.value_) {
                return true;
            }
        }
        return false;
    }

    template <typename... Args>
    void append(Args&&... args) {
        auto* new_node = creator_(std::forward<Args>(args)...);

        if (size_ == 0) {
            head_ = new_node;
        } else {
            tail_->next_ = new_node;
        }
        tail_ = new_node;
        ++size_;
    }

    void clear() {
        auto* cur = head_;
        Node* next_node = nullptr;
        while (cur) {
            next_node = cur->next_;
            alloc_.destroy(cur);
            cur = next_node;
        }
        head_ = tail_ = nullptr;
        size_ = 0;
    }

    value_t& operator[](usize index) {
        index = neg_index(index, size_);

        auto* cur = head_;
        while (index--) {
            cur = cur->next_;
        }
        return cur->value_;
    }

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
    C creator_;
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

template <typename T>
using ChainList = Chain<ChainNode<T>, Creator<ChainNode<T>>, Allocator<ChainNode<T>>>;

} // namespace my::util

#endif // CHAIN_HPP