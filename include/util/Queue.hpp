/**
 * @brief 队列，使用带尾指针的循环单链表实现
 * @author Ricky
 * @date 2025/1/13
 * @version 1.0
 */
#ifndef QUEUE_HPP
#define QUEUE_HPP

#include "ChainNode.hpp"
#include "Creator.hpp"

namespace my::util {

template <ChainNodeType Node, typename C = Creator<Node>>
class ChainQueue {
    using self = ChainQueue<Node, C>;

public:
    using value_t = typename Node::value_t;

    ChainQueue() :
            size_(0), tail_(new ChainNode<value_t>()) {
        tail_->next_ = tail_;
    }

    ~ChainQueue() {
        clear();
        my_destroy(tail_);
    }

    c_size size() const {
        return size_;
    }

    c_size empty() const {
        return tail_->next_ == tail_;
    }

    void clear() {
        auto* p = tail_->next_;
        auto* d = p->next_; // 待删节点
        while(d != p) {
            p->next_ = d->next_;
            d = p->next_;
        }
        tail_ = p;
        size_ = 0;
    }

    /**
     * @brief 入队
     * @note 时间复杂度 O(1)，采用尾插
     */
    template <typename... Args>
    void push(Args&&... args) {
        auto* newNode = creator_(std::forward<Args>(args)...);
        newNode->next_ = tail_->next_;
        tail_->next_ = newNode;
        tail_ = tail_->next_;
        ++size_;
    }

    /**
     * @brief 出队
     * @note 时间复杂度 O(1)，采用头删
     */
    void pop() {
        if (empty()) {
            RuntimeError("Queue is empty.");
            return;
        }
        auto* p = tail_->next_;
        auto* d = p->next_; // 待删节点
        if (d == tail_) {
            tail_ = p;
        }
        p->next_ = d->next_;
        my_destroy(d);
        --size_;
    }

    value_t& front() {
        if (empty()) {
            RuntimeError("Queue is empty.");
            return None<value_t>;
        }
        return tail_->next_->next_->value_;
    }

    const value_t& front() const {
        if (empty()) {
            RuntimeError("Queue is empty.");
            return None<value_t>;
        }
        return tail_->next_->next_->value_;
    }

    value_t& tail() {
        if (empty()) {
            RuntimeError("Queue is empty.");
            return None<value_t>;
        }
        return tail_->value_;
    }

    const value_t& tail() const {
        if (empty()) {
            RuntimeError("Queue is empty.");
            return None<value_t>;
        }
        return tail_->value_;
    }

private:
    c_size size_;              // 队列长度
    ChainNode<value_t>* tail_; // 指向虚拟尾节点的指针
    C creator_;                // 节点管理器
};

template <typename T>
using Queue = ChainQueue<ChainNode<T>, Creator<ChainNode<T>>>;

} // namespace my::util

#endif // QUEUE_HPP