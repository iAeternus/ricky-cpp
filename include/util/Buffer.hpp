/**
 * @brief 内存块
 * @author Ricky
 * @date 2024/12/2
 * @version 1.0
 */
#ifndef BUFFER_HPP
#define BUFFER_HPP

#include "Array.hpp"

namespace my::util {

template <typename T, typename Alloc = Allocator<T>>
class Buffer : public Sequence<Buffer<T>, T> {
public:
    using value_t = T;
    using Self = Buffer<value_t, Alloc>;
    using Super = Sequence<Buffer<value_t>, value_t>;

    using iterator = Super::iterator;
    using const_iterator = Super::const_iterator;

    Buffer() :
            size_(0), capacity_(0), buf_(nullptr) {}

    Buffer(usize capacity) :
            size_(0), capacity_(capacity), buf_(alloc_.allocate(capacity_)) {}

    Buffer(const Self& other) :
            alloc_(other.alloc_), size_(other.size_), capacity_(other.capacity_), buf_(alloc_.allocate(capacity_)) {
        for (usize i = 0; i < other.size(); ++i) {
            alloc_.construct(buf_ + i, other.buf_[i]);
        }
    }

    Buffer(Self&& other) noexcept :
            alloc_(other.alloc_), size_(other.size_), capacity_(other.capacity_), buf_(other.buf_) {
        other.size_ = other.capacity_ = 0;
        other.buf_ = nullptr;
    }

    Self& operator=(const Self& other) {
        if (this == &other) return *this;
        alloc_.destroy(this);
        alloc_.construct(this, other);
        return *this;
    }

    Self& operator=(Self&& other) noexcept {
        if (this == &other) return *this;

        alloc_.destroy(this);
        alloc_.construct(this, std::move(other));
        return *this;
    }

    ~Buffer() {
        alloc_.destroy(buf_, size_);
        alloc_.deallocate(buf_, size_);
        size_ = capacity_ = 0;
    }

    usize size() const {
        return size_;
    }

    usize capacity() const {
        return capacity_;
    }

    bool full() const {
        return size_ == capacity_;
    }

    value_t* data() {
        return buf_;
    }

    const value_t* data() const {
        return buf_;
    }

    value_t& front() {
        return buf_[0];
    }

    const value_t& front() const {
        return buf_[0];
    }

    value_t& back() {
        return buf_[size_ - 1];
    }

    const value_t& back() const {
        return buf_[size_ - 1];
    }

    /**
     * @brief 在buffer末尾追加元素，不会做容量检查
     * @return 返回追加的元素
     */
    template <typename... Args>
    value_t& append(Args&&... args) {
        alloc_.construct(buf_ + size_, std::forward<Args>(args)...);
        return buf_[size_++];
    }

    void pop_back() {
        alloc_.destroy(buf_ + size_);
        --size_;
    }

    /**
     * @brief 重新分配内存
     */
    void resize(usize new_capacity) {
        if (new_capacity == capacity_) return;
        alloc_.destroy(this);
        alloc_.construct(this, new_capacity);
    }

    value_t& at(usize index) {
        return buf_[index];
    }

    const value_t& at(usize index) const {
        return buf_[index];
    }

    /**
     * @brief 转换为Array，拷贝
     */
    Array<value_t> to_array() const {
        Array<value_t> arr(size_);
        for (usize i = 0; i < size_; ++i) {
            arr[i] = at(i);
        }
        return arr;
    }

    /**
     * @brief 转换为Array，移动
     */
    Array<value_t> to_array() {
        Array<value_t> arr(size_);
        for (usize i = 0; i < size_; ++i) {
            arr[i] = std::move(at(i));
        }
        size_ = 0;
        return arr;
    }

private:
    Alloc alloc_{};
    usize size_, capacity_;
    value_t* buf_;
};

} // namespace my::util

#endif // BUFFER_HPP