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
    using super = Object<IndexIterator>;
    using self = IndexIterator;

public:
    using container_t = std::conditional_t<IsConst, const C, C>;
    using iterator_category = std::random_access_iterator_tag;
    using value_type = std::conditional_t<IsConst, const V, V>;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;

    IndexIterator() :
            container_(nullptr), index_(0) {}

    IndexIterator(container_t* container, c_size index) :
            container_(container ? std::shared_ptr<container_t>(container) : nullptr), index_(index) {}

    IndexIterator(const self& other) :
            IndexIterator(other.container_, other.index_) {}

    self& operator=(const self& other) {
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

    self& operator++() {
        ++index_;
        return *this;
    }

    self operator++(int) {
        self tmp(*this);
        ++index_;
        return tmp;
    }

    self& operator--() {
        --index_;
        return *this;
    }

    self operator--(int) {
        self tmp(*this);
        --index_;
        return tmp;
    }

    self operator+(difference_type n) const {
        return self(container_, index_ + n);
    }

    self operator-(difference_type n) const {
        return self(container_, index_ - n);
    }

    self& operator+=(difference_type n) const {
        index_ += n;
        return *this;
    }

    self& operator-=(difference_type n) const {
        index_ -= n;
        return *this;
    }

    difference_type operator-(const self& other) const {
        return index_ - other.index_;
    }

    bool __equals__(const self& other) const {
        return container_ == other.container_ && index_ == other.index_;
    }

private:
    std::shared_ptr<container_t> container_;
    c_size index_;
};

} // namespace my::util

#endif // INDEX_ITERATOR_HPP
