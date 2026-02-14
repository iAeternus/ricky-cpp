/**
 * @file Queue.hpp TODO 将弃用
 * @brief 基于带尾指针的循环单链表实现的队列类
 * @author Ricky
 * @date 2025/1/13
 * @version 1.0
 */
#ifndef QUEUE_HPP
#define QUEUE_HPP

#include "my_exception.hpp"
#include "object.hpp"

namespace my::util {

/**
 * @brief 链节点类型约束
 */
template <typename T>
concept ChainNodeType = requires(T a, const T& b, T&& c) {
    { T() } -> std::same_as<T>;
    { a = b } -> std::same_as<T&>;
    { T(b) } -> std::same_as<T>;
    { a = std::move(c) } -> std::same_as<T&>;
    { T(std::move(c)) } -> std::same_as<T>;
    { a.next_ };
    { a.value_ };
};

/**
 * @brief 链节点，只能前向移动
 */
template <typename T>
class ChainNode : public Object<ChainNode<T>> {
public:
    using value_t = T;
    using Self = ChainNode<value_t>;
    using Super = Object<Self>;

    ChainNode(const value_t& value = {}, Self* next = nullptr) :
            value_(value), next_(next) {}

    ChainNode(const Self& other) :
            value_(other.value_), next_(other.next_) {}

    ChainNode(Self&& other) noexcept :
            value_(std::move(other.value_)), next_(other.next_) {
        other.next_ = nullptr;
    }

    Self& operator=(const Self& other) {
        this->value_ = other.value_;
        this->next_ = nullptr;
        return *this;
    }

    Self& operator=(Self&& other) noexcept {
        this->value_ = std::move(other.value_);
        this->next_ = other.next_;
        other.next_ = nullptr;
        return *this;
    }

    [[nodiscard]] CString __str__() const {
        std::stringstream stream;
        stream << "<Node  " << value_ << '>';
        return CString(stream.str());
    }

    [[nodiscard]] bool __equals__(const Self& other) const {
        return this->value_ == other.value_;
    }

    bool operator==(const Self& other) const {
        return this->__equals__(other);
    }

    bool operator!=(const Self& other) const {
        return !this->__equals__(other);
    }

    value_t value_;
    Self* next_;
};

/**
 * @class ChainQueue
 * @brief 基于带尾指针的循环单链表实现的队列
 * @tparam Node 指定链表节点类型
 * @tparam Alloc 内存分配器类型
 */
template <ChainNodeType Node, typename Alloc = mem::Allocator<Node>>
class ChainQueue : public Object<ChainQueue<Node, Alloc>> {
public:
    using Self = ChainQueue<Node, Alloc>;
    using value_t = typename Node::value_t;

    /**
     * @brief 构造函数，初始化队列
     */
    ChainQueue() :
            size_(0), tail_(alloc_.create()) {
        tail_->next_ = tail_;
    }

    /**
     * @brief 析构函数，释放队列中的所有节点
     */
    ~ChainQueue() {
        clear();
        alloc_.destroy(tail_);
        alloc_.deallocate(tail_, 1);
    }

    /**
     * @brief 获取队列的大小
     * @return 队列中元素的个数
     */
    usize size() const {
        return size_;
    }

    /**
     * @brief 判断队列是否为空
     * @return 队列为空时返回 `true`，否则返回 `false`
     */
    bool empty() const {
        return tail_->next_ == tail_;
    }

    /**
     * @brief 清空队列，释放所有节点
     */
    void clear() {
        auto* p = tail_->next_;
        while (p != tail_) {
            auto* d = p;
            p = p->next_;
            alloc_.destroy(d);
            alloc_.deallocate(d, 1);
        }
        tail_->next_ = tail_;
        size_ = 0;
    }

    /**
     * @brief 入队操作，将元素添加到队列尾部
     * @tparam Args 入队元素的参数类型
     * @param args 入队元素的参数
     * @note 时间复杂度为 O(1)，采用尾插法
     */
    template <typename... Args>
    void push(Args&&... args) {
        auto* new_node = alloc_.create(std::forward<Args>(args)...);
        new_node->next_ = tail_->next_;
        tail_->next_ = new_node;
        tail_ = new_node;
        ++size_;
    }

    /**
     * @brief 出队操作，移除队列头部元素
     * @note 时间复杂度为 O(1)，采用头删法
     * @exception Exception 若队空，则抛出 runtime_exception
     */
    void pop() {
        if (empty()) {
            throw runtime_exception("Queue is empty.");
        }
        auto* p = tail_->next_;
        auto* d = p->next_; // 待删除节点
        if (d == tail_) {   // 如果队列仅剩一个节点
            tail_ = p;      // 更新尾节点为起始节点
        }
        p->next_ = d->next_;
        alloc_.destroy(d);
        alloc_.deallocate(d, 1);
        --size_;
    }

    /**
     * @brief 获取队列首元素的引用
     * @return 队首元素的引用
     * @note 如果队列为空，会抛出异常
     * @exception Exception 若队空，则抛出 runtime_exception
     */
    value_t& front() {
        if (empty()) {
            throw runtime_exception("Queue is empty.");
        }
        return tail_->next_->next_->value_;
    }

    /**
     * @brief 获取队列首元素的常量引用
     * @return 队首元素的常量引用
     * @note 如果队列为空，会抛出异常
     * @exception Exception 若队空，则抛出 runtime_exception
     */
    const value_t& front() const {
        if (empty()) {
            throw runtime_exception("Queue is empty.");
        }
        return tail_->next_->next_->value_;
    }

    /**
     * @brief 获取队列尾元素的引用
     * @return 队尾元素的引用
     * @note 如果队列为空，会抛出异常
     * @exception Exception 若队空，则抛出 runtime_exception
     */
    value_t& tail() {
        if (empty()) {
            throw runtime_exception("Queue is empty.");
        }
        return tail_->value_;
    }

    /**
     * @brief 获取队列尾元素的常量引用
     * @return 队尾元素的常量引用
     * @note 如果队列为空，会抛出异常
     * @exception Exception 若队空，则抛出 runtime_exception
     */
    const value_t& tail() const {
        if (empty()) {
            throw runtime_exception("Queue is empty.");
        }
        return tail_->value_;
    }

private:
    Alloc alloc_{}; // 内存分配器
    usize size_;    // 队列中元素的个数
    Node* tail_;    // 指向虚拟尾节点的指针
};

/**
 * @brief 队列类型，节点类型为 `ChainNode<T>`
 * @tparam T 节点存储的值类型
 */
template <typename T, typename Alloc = mem::Allocator<ChainNode<T>>>
using Queue = ChainQueue<ChainNode<T>, Alloc>;

} // namespace my::util

#endif // QUEUE_HPP