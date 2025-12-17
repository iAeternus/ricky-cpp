/**
 * @brief 双向链表
 * @author Ricky
 * @date 2025/12/16
 * @version 1.0
 */
#ifndef LINKED_LIST_HPP
#define LINKED_LIST_HPP

#include "object.hpp"

namespace my::util {

/**
 * @brief 双向链表节点类型约束
 */
template <typename T>
concept LinkedListNodeType = requires(T a, const T& b, T&& c) {
    typename T::value_t;
    { T(b) } -> std::same_as<T>;
    { T(std::move(c)) } -> std::same_as<T>;
    { a = b } -> std::same_as<T&>;
    { a = std::move(c) } -> std::same_as<T&>;
    { a.elem };
    { a.next };
    { a.prev };
};

/**
 * @brief 双向链表节点
 */
template <typename T>
struct LinkedListNode {
    using Self = LinkedListNode<T>;
    using value_t = T;

    T elem;
    Self* next;
    Self* prev;

    LinkedListNode(const value_t& elem, Self* next = nullptr, Self* prev = nullptr) :
            elem(elem), next(next), prev(prev) {}

    LinkedListNode(const Self& other) :
            elem(other.elem), next(other.next), prev(other.prev) {}

    LinkedListNode(Self&& other) noexcept :
            elem(std::move(other.elem)), next(other.next), prev(other.prev) {
        other.next = other.prev = nullptr;
    }

    Self& operator=(const Self& other) {
        if (this == &other) return *this;

        this->elem = other.elem;
        this->next = other.next;
        this->prev = other.prev;
        return *this;
    }

    Self& operator=(Self&& other) noexcept {
        if (this == &other) return *this;

        this->elem = std::move(other.elem);
        this->next = other.next;
        this->prev = other.prev;
        other.next = other.prev = nullptr;
        return *this;
    }
};

/**
 * @brief 双向链表迭代器
 */
template <LinkedListNodeType Node>
class ListIterator {
public:
    using Self = ListIterator<Node>;
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = typename Node::value_t;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

    ListIterator(Node* node, Node* sentinel) :
            node_(node), sentinel_(sentinel) {}

    reference operator*() const {
        return node_->elem;
    }

    pointer operator->() const {
        return &node_->elem;
    }

    Self& operator++() {
        node_ = node_->next;
        return *this;
    }

    Self operator++(int) {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    Self& operator--() {
        node_ = node_->prev;
        return *this;
    }

    Self operator--(int) {
        auto tmp = *this;
        --(*this);
        return tmp;
    }

    friend bool operator==(const Self& a, const Self& b) {
        return a.node_ == b.node_;
    }

    friend bool operator!=(const Self& a, const Self& b) {
        return !(a == b);
    }

private:
    Node* node_;
    Node* sentinel_;
};

/**
 * @brief 带头尾指针的循环双向链表
 */
template <LinkedListNodeType Node, typename Alloc = mem::Allocator<Node>>
class LinkedListImpl : public Object<LinkedListImpl<Node, Alloc>> {
public:
    using Self = LinkedListImpl<Node, Alloc>;
    using value_t = typename Node::value_t;
    using iterator = ListIterator<Node>;
    using const_iterator = ListIterator<Node>;

    LinkedListImpl() :
            sentinel_(alloc_.create(value_t{})), size_(0) {
        sentinel_->next = sentinel_;
        sentinel_->prev = sentinel_;
    }

    ~LinkedListImpl() {
        clear();
        alloc_.destroy(sentinel_);
        alloc_.deallocate(sentinel_, 1);
    }

    bool is_empty() const noexcept {
        return size_ == 0;
    }

    size_t size() const noexcept {
        return size_;
    }

    bool is_sentinel(const value_t& value) const noexcept {
        return sentinel_->elem == value;
    }

    value_t& front() const noexcept {
        return sentinel_->next->elem;
    }

    value_t& front() noexcept {
        return sentinel_->next->elem;
    }

    value_t& back() const noexcept {
        return sentinel_->prev->elem;
    }

    value_t& back() noexcept {
        return sentinel_->prev->elem;
    }

    void clear() {
        while (!is_empty()) {
            pop_front();
        }
    }

    template <typename... Args>
    void push_front(Args&&... args) {
        Node* node = alloc_.create(std::forward<Args>(args)...);
        link_between(sentinel_, node, sentinel_->next);
        ++size_;
    }

    template <typename... Args>
    void push_back(Args&&... args) {
        Node* node = alloc_.create(std::forward<Args>(args)...);
        link_between(sentinel_->prev, node, sentinel_);
        ++size_;
    }

    void pop_front() {
        if (is_empty()) return;
        erase(sentinel_->next);
    }

    void pop_back() {
        if (is_empty()) return;
        erase(sentinel_->prev);
    }

    void erase(Node* node) {
        if (node == sentinel_) return;

        unlink(node);
        alloc_.destroy(node);
        alloc_.deallocate(node, 1);
        --size_;
    }

    bool contains(const value_t& value) const {
        for (auto&& elem : *this) {
            if (elem == value) {
                return true;
            }
        }
        return false;
    }

    iterator begin() { return {sentinel_->next, sentinel_}; }
    iterator end() { return {sentinel_, sentinel_}; }

    const_iterator begin() const { return {sentinel_->next, sentinel_}; }
    const_iterator end() const { return {sentinel_, sentinel_}; }

    [[nodiscard]] CString __str__() const {
        std::stringstream stream;
        bool is_first = true;
        stream << '[';
        for (auto&& elem : *this) {
            if (is_first) {
                is_first = false;
            } else {
                stream << ',';
            }
            stream << elem;
        }
        stream << ']';
        return CString{stream.str()};
    }

private:
    static void link_between(Node* prev, Node* node, Node* next) noexcept {
        node->prev = prev;
        node->next = next;
        prev->next = node;
        next->prev = node;
    }

    static void unlink(Node* node) noexcept {
        node->prev->next = node->next;
        node->next->prev = node->prev;
        node->next = node->prev = nullptr;
    }

private:
    Node* sentinel_; // 哨兵节点
    usize size_;
    Alloc alloc_{};
};

template <typename T>
using LinkedList = LinkedListImpl<LinkedListNode<T>, mem::Allocator<LinkedListNode<T>>>;

} // namespace my::util

#endif // LINKED_LIST_HPP
