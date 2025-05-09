/**
 * @brief 内存块
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
public:
    using value_t = T;
    using Self = Buffer<value_t>;
    using Super = Sequence<Buffer<value_t>, value_t>;

    using iterator = Super::iterator;
    using const_iterator = Super::const_iterator;

    Buffer() :
            size_(0), capacity_(0), buf_(nullptr) {}

    Buffer(isize capacity) :
            size_(0), capacity_(capacity), buf_(my_alloc<value_t>(capacity_)) {}

    Buffer(const Self& other) :
            size_(other.size_), capacity_(other.capacity_), buf_(my_alloc<value_t>(capacity_)) {
        for (isize i = 0; i < other.size(); ++i) {
            my_construct(data() + i, other.data()[i]);
        }
    }

    Buffer(Self&& other) noexcept :
            size_(other.size_), capacity_(other.capacity_), buf_(other.buf_) {
        other.size_ = other.capacity_ = 0;
        other.buf_ = nullptr;
    }

    Self& operator=(const Self& other) {
        if (this == &other) return *this;
        my_destroy(this);
        return *my_construct(this, other);
    }

    Self& operator=(Self&& other) noexcept {
        if (this == &other) return *this;
        my_destroy(this);
        return *my_construct(this, std::move(other));
    }

    ~Buffer() {
        my_destroy(buf_, size_);
        my_delloc(buf_);
        size_ = capacity_ = 0;
    }

    isize size() const {
        return size_;
    }

    isize capacity() const {
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
        my_construct(buf_ + size_, std::forward<Args>(args)...);
        return buf_[size_++];
    }

    void pop_back() {
        my_destroy(buf_ + size_);
        --size_;
    }

    /**
     * @brief 重新分配内存
     */
    void resize(isize new_capacity) {
        if (new_capacity == capacity_) return;
        my_destroy(this);
        my_construct(this, new_capacity);
    }

    value_t& at(isize index) {
        return buf_[index];
    }

    const value_t& at(isize index) const {
        return buf_[index];
    }

    /**
     * @brief 转换为Array，拷贝
     */
    Array<value_t> to_array() const {
        Array<value_t> arr(size_);
        for (isize i = 0; i < size_; ++i) {
            arr[i] = at(i);
        }
        return arr;
    }

    /**
     * @brief 转换为Array，移动
     */
    Array<value_t> to_array() {
        Array<value_t> arr(size_);
        for (isize i = 0; i < size_; ++i) {
            arr[i] = std::move(at(i));
        }
        size_ = 0;
        return arr;
    }

private:
    isize size_, capacity_;
    value_t* buf_;
};

} // namespace my::util

#endif // BUFFER_HPP