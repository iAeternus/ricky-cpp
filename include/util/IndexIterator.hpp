/**
 * @brief 索引容器迭代器
 * @author Ricky
 * @date 2024/12/1
 * @version 1.0
 */
#ifndef INDEX_ITERATOR_HPP
#define INDEX_ITERATOR_HPP

#include "Object.hpp"

namespace my::util {

template <bool IsConst, typename C, typename V>
class IndexIterator : public Object<IndexIterator<IsConst, C, V>> {
    using Super = Object<IndexIterator>;
    using Self = IndexIterator;

public:
    using container_t = std::conditional_t<IsConst, const C, C>;
    using iterator_category = std::random_access_iterator_tag;
    using value_type = std::conditional_t<IsConst, const V, V>;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;

    IndexIterator(container_t* container = nullptr, isize index = 0) :
            container_(container), index_(index) {}

    IndexIterator(const Self& other) :
            IndexIterator(other.container_, other.index_) {}

    Self& operator=(const Self& other) {
        if (this != &other) {
            this->container_ = other.container_;
            this->index_ = other.index_;
        }
        return *this;
    }

    reference operator*() {
        return container_->operator[](index_);
    }

    const_reference operator*() const {
        return container_->operator[](index_);
    }

    pointer operator->() {
        return &container_->operator[](index_);
    }

    const_pointer operator->() const {
        return &container_->operator[](index_);
    }

    Self& operator++() {
        ++index_;
        return *this;
    }

    Self operator++(int) {
        Self tmp(*this);
        ++index_;
        return tmp;
    }

    Self& operator--() {
        --index_;
        return *this;
    }

    Self operator--(int) {
        Self tmp(*this);
        --index_;
        return tmp;
    }

    Self operator+(difference_type n) const {
        return Self(container_, index_ + n);
    }

    Self operator-(difference_type n) const {
        return Self(container_, index_ - n);
    }

    Self& operator+=(difference_type n) {
        index_ += n;
        return *this;
    }

    Self& operator-=(difference_type n) {
        index_ -= n;
        return *this;
    }

    difference_type operator-(const Self& other) const {
        return index_ - other.index_;
    }

    bool __equals__(const Self& other) const {
        return container_ == other.container_ && index_ == other.index_;
    }

    bool operator==(const Self& other) const {
        return __equals__(other);
    }

    bool operator!=(const Self& other) const {
        return !__equals__(other);
    }

private:
    container_t* container_;
    isize index_;
};

} // namespace my::util

#endif // INDEX_ITERATOR_HPP
