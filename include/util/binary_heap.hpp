/**
 * @brief 优先队列
 * @author Ricky
 * @date 2025/7/7
 * @version 1.0
 */
#ifndef PRIORITY_QUEUE_HPP
#define PRIORITY_QUEUE_HPP

#include "vec.hpp"

namespace my::util {

/**
 * @class BinaryHeap
 * @brief 二叉堆实现优先队列
 * @tparam T 元素类型
 * @tparam Comp 比较二元函数，定义第一个参数优先级高
 * @tparam Alloc 内存分配器类型
 */
template <typename T, typename Comp = std::less<T>, typename Alloc = mem::Allocator<T>>
class BinaryHeap : public Object<BinaryHeap<T, Comp, Alloc>> {
public:
    using value_t = T;
    using Self = BinaryHeap<value_t, Comp, Alloc>;

    /**
     * @brief 带比较器的构造函数
     * @param comp 自定义比较器
     */
    explicit BinaryHeap(Comp comp = Comp{}) :
            heap_{}, comp_(comp) {}

    /**
     * @brief 带初始容量和比较器的构造函数
     * @param cap 初始容量
     * @param comp 自定义比较器
     */
    explicit BinaryHeap(usize cap, Comp comp = Comp{}) :
            heap_(cap), comp_(std::move(comp)) {}

    /**
     * @brief 将一个可迭代对象堆化
     * @note 时间复杂度 O(n)
     * @tparam I 可迭代对象类型
     * @param iter 可迭代对象
     * @param comp 自定义比较器
     */
    template <Iterable I>
    explicit BinaryHeap(I iter, Comp comp = Comp{}) :
            heap_(iter), comp_(comp) {
        heapify();
    }

    /**
     * @brief 检查队列是否为空
     * @return 如果队列为空返回 true，否则返回 false
     */
    bool empty() const noexcept {
        return heap_.empty();
    }

    /**
     * @brief 获取队列大小
     * @return 队列中元素的数量
     */
    usize size() const noexcept {
        return heap_.size();
    }

    /**
     * @brief 获取当前容量
     * @return 当前分配的存储容量
     */
    size_t capacity() const noexcept {
        return heap_.capacity();
    }

    /**
     * @brief 插入元素
     * @param val 要插入的元素
     */
    template <typename V>
    void push(V&& val) {
        heap_.append(std::forward<V>(val));
        heapify_up(heap_.size() - 1);
    }

    /**
     * @brief 构造并插入元素
     * @param args 构造元素的参数
     */
    template <typename... Args>
    void push(Args&&... args) {
        heap_.append(std::forward<Args>(args)...);
        heapify_up(heap_.size() - 1);
    }

    /**
     * @brief 移除堆顶元素
     */
    void pop() {
        if (empty()) return;

        heap_.front() = std::move(heap_.back());
        heap_.pop();
        if (!empty()) {
            heapify_down(0);
        }
    }

    const T& top() const {
        return heap_.front();
    }

    /**
     * @brief 清空队列
     */
    void clear() noexcept {
        heap_.clear();
    }

    /**
     * @brief 交换两个优先队列的内容
     * @param other 要交换的另一个优先队列
     */
    void swap(Self& other) noexcept {
        heap_.swap(other.heap_);
        std::swap(comp_, other.comp_);
    }

    /**
     * @brief 预留存储空间
     * @param new_cap 新的容量
     */
    void reserve(usize new_cap) {
        heap_.reserve(new_cap);
    }

    /**
     * @brief 仅用于debug
     */
    CString __str__() const {
        if (heap_.empty()) {
            return "[]";
        }
        std::stringstream stream;
        stream << '[' << heap_[0];
        for (usize i = 1; i < heap_.size(); ++i) {
            stream << ',' << heap_[i];
        }
        stream << ']';
        return CString{stream.str()};
    }

private:
    /**
     * @brief 获取父节点索引
     */
    static usize fa(const usize i) {
        return (i - 1) >> 1;
    }

    /**
     * @brief 获取左子节点索引
     */
    static usize lch(const usize i) {
        return (i << 1) + 1;
    }

    /**
     * @brief 获取右子节点索引
     */
    static usize rch(const usize i) {
        return (i << 1) + 2;
    }

    /**
     * @brief 上浮操作（堆化向上）
     * @param idx 开始上浮的索引
     */
    void heapify_up(usize idx) {
        while (idx > 0) {
            const auto fa_idx = fa(idx);
            // 如果当前节点优于父节点，则交换
            if (comp_(heap_[idx], heap_[fa_idx])) {
                std::swap(heap_[idx], heap_[fa_idx]);
                idx = fa_idx;
            } else {
                break;
            }
        }
    }

    /**
     * @brief 下沉操作（堆化向下）
     * @param idx 开始下沉的索引
     */
    void heapify_down(usize idx) {
        const auto m_size = heap_.size();
        auto curr = idx;

        loop {
            const auto l = lch(idx), r = rch(idx);

            // 检查左子节点
            if (l < m_size && comp_(heap_[l], heap_[curr])) {
                curr = l;
            }

            // 检查右子节点
            if (r < m_size && comp_(heap_[r], heap_[curr])) {
                curr = r;
            }

            // 如果当前节点是最小的，则停止下沉
            if (curr == idx) break;

            std::swap(heap_[idx], heap_[curr]);
            idx = curr;
        }
    }

    /**
     * @brief 将数组堆化
     * @param vec 数组
     */
    void heapify() {
        const auto n = heap_.size();
        if (n <= 1) return;
        for (i64 i = (n >> 1) - 1; i >= 0; --i) {
            heapify_down(static_cast<usize>(i));
        }
    }

private:
    Vec<T, Alloc> heap_; // 堆容器
    Comp comp_;          // 比较函数，默认为std::less，即小根堆
};

} // namespace my::util

#endif // PRIORITY_QUEUE_HPP