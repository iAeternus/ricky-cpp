/**
 * @brief 缓冲区
 * @author Ricky
 * @date 2024/12/2
 * @version 1.0
 */
#ifndef BUFFER_HPP
#define BUFFER_HPP

#include "raise_error.hpp"
#include "ricky_memory.hpp"
#include "Array.hpp"
#include "RelationIterator.hpp"

namespace my::util {

template <typename T>
class Buffer : public Sequence<Buffer<T>, T> {
    using self = Buffer<T>;
    using super = Sequence<Buffer<T>, T>;

public:
    using value_t = T;
    using iterator = super::iterator;
    using const_iterator = super::const_iterator;

    Buffer() :
            size_(0), capacity_(0), buffer_(nullptr) {}

    Buffer(c_size capacity) :
            size_(0), capacity_(capacity), buffer_(my_alloc<value_t>(capacity_)) {}

    Buffer(const Buffer& other) :
            size_(other.size_), capacity_(other.capacity_), buffer_(my_alloc<value_t>(capacity_)) {
        for (c_size i = 0; i < other.size(); ++i) {
            my_construct(data() + i, other.data()[i]);
        }
    }

    Buffer(Buffer&& other) noexcept :
            size_(other.size_), capacity_(other.capacity_), buffer_(other.buffer_) {
        other.size_ = other.capacity_ = 0;
        other.buffer_ = nullptr;
    }

    self& operator=(const self& other) {
        if (this == &other) return *this;
        my_destroy(this);
        return *my_construct(this, other);
    }

    self& operator=(self&& other) noexcept {
        if (this == &other) return *this;
        my_destroy(this);
        return *my_construct(this, std::move(other));
    }

    ~Buffer() {
        my_destroy(buffer_, size());
        my_delloc(buffer_);
        size_ = capacity_ = 0;
    }

    c_size size() const {
        return size_;
    }

    c_size capacity() const {
        return capacity_;
    }

    bool full() const {
        return size_ == capacity_;
    }

    value_t* data() {
        return buffer_;
    }

    const value_t* data() const {
        return buffer_;
    }

    /**
     * @brief 在buffer末尾追加元素，不会做容量检查
     * @return 返回追加的元素
     */
    template <typename... Args>
    value_t& append(Args&&... args) {
        my_construct(buffer_ + size_, std::forward<Args>(args)...);
        return buffer_[size_++];
    }

    void pop_back() {
        my_destroy(buffer_ + size_);
        --size_;
    }

    /**
     * @brief 重新分配内存
     */
    void resize(int newSize) {
        my_destroy(this);
        my_construct(this, newSize);
    }

    value_t& at(c_size index) {
        return buffer_[index];
    }

    const value_t& at(c_size index) const {
        return buffer_[index];
    }

    /**
     * @brief 转换为Array，拷贝
     */
    Array<value_t> toArray() const {
        Array<value_t> arr(size_);
        for (c_size i = 0; i < size_; ++i) {
            arr[i] = at(i);
        }
        return arr;
    }

    /**
     * @brief 转换为Array，移动
     */
    Array<value_t> toArray() {
        Array<value_t> arr(size_);
        for (c_size i = 0; i < size_; ++i) {
            arr[i] = std::move(at(i));
        }
        size_ = 0;
        return arr;
    }

private:
    c_size size_, capacity_;
    value_t* buffer_;
};

} // namespace my::util

#endif // BUFFER_HPP