/**
 * @brief 链容器
 * @author Ricky
 * @date 2024/12/14
 * @version 1.0
 */
#ifndef CHAIN_HPP
#define CHAIN_HPP

#include "Creator.hpp"
#include "ChainNode.hpp"

namespace my::util {

template <ChainNodeType Node>
class ChainIterator;

/**
 * @brief 链
 */
template <ChainNodeType Node, typename C = Creator<Node>>
class Chain : public Object<Chain<Node, C>> {
    using self = Chain<Node, C>;
    using super = Object<self>;

public:
    friend class ChainIterator<Node>;
    using value_t = typename Node::value_t;

    Chain() :
            head_(nullptr), tail_(nullptr), size_(0) {}

    virtual ~Chain() {
        clear();
    }

    constexpr c_size size() const {
        return size_;
    }

    constexpr bool empty() const {
        return size_ == 0;
    }

    bool contains(const Node& node) const {
        for (const auto& current : *this) {
            if (current == node.value_) {
                return true;
            }
        }
        return false;
    }

    template <typename... Args>
    void append(Args&&... args) {
        Node* newNode = creator_(std::forward<Args>(args)...);

        if (size_ == 0) {
            head_ = newNode;
        } else {
            tail_->next_ = newNode;
        }
        tail_ = newNode;
        ++size_;
    }

    void clear() {
        Node* current = head_;
        Node* nextNode = nullptr;
        while (current) {
            nextNode = current->next_;
            my_destroy(current);
            current = nextNode;
        }
        head_ = tail_ = nullptr;
        size_ = 0;
    }

    value_t& operator[](c_size index) {
        index = neg_index(index, size_);

        Node* current = head_;
        while (index--) {
            current = current->next_;
        }
        return current->value_;
    }

    const value_t& operator[](c_size index) const {
        index = neg_index(index, size_);

        Node* current = head_;
        while (index--) {
            current = current->next_;
        }
        return current->value_;
    }

    /**
     * @brief 转换为Array，拷贝
     */
    Array<value_t> toArray() const {
        Array<value_t> arr(size_);
        c_size pos = 0;
        for (const auto& current : *this) {
            arr[pos++] = current;
        }
        return arr;
    }

    /**
     * @brief 转换为Array，移动
     */
    Array<value_t> toArray() {
        Array<value_t> arr(size_);
        c_size pos = 0;
        for (auto&& current : *this) {
            arr[pos++] = std::forward<value_t>(current);
        }
        clear();
        return arr;
    }

    CString __str__() const {
        std::stringstream stream;
        stream << "<Chain> [";
        for (const auto& current : *this) {
            stream << current;
            if (current.next) {
                stream << "->";
            }
        }
        stream << ']';
        return CString(stream.str());
    }

    using iterator = ChainIterator<Node>;

    iterator begin() const {
        return iterator{head_};
    }

    iterator end() const {
        return iterator{nullptr};
    }

protected:
    Node *head_, *tail_;
    c_size size_;
    C creator_;
};

/**
 * @brief 链迭代器
 */
template <ChainNodeType Node>
class ChainIterator : public Object<ChainIterator<Node>> {
    using self = ChainIterator<Node>;

public:
    using value_t = typename Node::value_t;

    ChainIterator(Node* node = nullptr) :
            current_(node) {}

    ChainIterator(const self& other) :
            current_(other.current_) {}

    value_t& operator*() const {
        return current_->value_;
    }

    value_t* operator->() const {
        return &current_->value_;
    }

    self& operator++() {
        current_ = current_->next_;
        return *this;
    }

    self operator++(int) {
        self tmp{current_};
        current_ = current_->next_;
        return tmp;
    }

    cmp_t __cmp__(const self& other) const {
        return this->current_ - other.current_;
    }

    bool operator==(const self& other) const {
        return this->__cmp__(other) == 0;
    }

    bool operator!=(const self& other) const {
        return this->__cmp__(other) != 0;
    }

protected:
    Node* current_;
};

} // namespace my::util

#endif // CHAIN_HPP