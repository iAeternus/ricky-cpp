/**
 * @brief 基于策略模式的迭代器封装
 * @author Ricky
 * @date 2024/12/2
 * @version 1.0
 */
#ifndef RELATION_ITERATOR_HPP
#define RELATION_ITERATOR_HPP

#include "ricky_concepts.hpp"
#include "raise_error.hpp"

namespace my::util {

/**
 * @brief 迭代器移动策略基类
 */
template <std::input_or_output_iterator I>
class BaseIterMove : public Object<BaseIterMove<I>> {
public:
    using iterator = I;
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = std::remove_reference<decltype(*std::declval<iterator>())>;

    static iterator& next(iterator& iter) {
        NotImplementedError("next not implemented");
        return None<I>;
    }

    static iterator& prev(iterator& iter) {
        NotImplementedError("prev not implemented");
        return None<I>;
    }
};

/**
 * @brief 自增
 */
template <std::input_or_output_iterator I>
class SelfAddMove : public BaseIterMove<I> {
    using self = SelfAddMove;
    using super = BaseIterMove<I>;

public:
    using iterator = I;
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = std::remove_reference<decltype(*std::declval<iterator>())>;

    /**
     * @brief 移动到下一个元素，修改自身
     */
    static iterator& next(iterator& iter) {
        return ++iter;
    }

    /**
     * @brief 移动到上一个元素，修改自身
     */
    static iterator& prev(iterator iter) {
        return --iter;
    }
};

template <typename IterMove>
class RelationIterator : public Object<RelationIterator<IterMove>> {
    using self = RelationIterator<IterMove>;
    using super = Object<self>;

public:
    using iterator = typename IterMove::iterator;
    using move_t = IterMove;
    using iterator_category = typename IterMove::iterator_category;
    using value_type = typename IterMove::value_type;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;

    RelationIterator() :
            iter_() {}

    RelationIterator(const iterator& iter) :
            iter_(iter) {}

    RelationIterator(const self& other) :
            iter_(other.iter_) {}

    RelationIterator(self&& other) noexcept :
            iter_(std::move(other.iter_)) {}

    self& operator=(const self& other) {
        if (this == &other) return *this;
        this->iter_ = other.iter_;
        return *this;
    }

    self& operator=(self&& other) noexcept {
        if (this == &other) return *this;
        this->iter_ = std::move(other.iter_);
        return *this;
    }

    reference operator*() {
        return *iter_;
    }

    const_reference operator*() const {
        return *iter_;
    }

    pointer operator->() {
        return &(*iter_);
    }

    const_pointer operator->() const {
        return &(*iter_);
    }

    self& operator++() {
        move_t::next(iter_);
        return *this;
    }

    self operator++(int) {
        iterator tmp(this);
        ++*this;
        return self{tmp};
    }

    self& operator--() {
        move_t::prev(iter_);
        return *this;
    }

    self operator-(int) {
        iterator tmp(this);
        --*this;
        return self{tmp};
    }

    bool __equals__(const self& other) const {
        return iter_ == other.iter_;
    }

    bool operator==(const self& other) const {
        return __equals__(other);
    }

    bool operator!=(const self& other) const {
        return !__equals__(other);
    }

private:
    iterator iter_;
};

} // namespace my::util

#endif // RELATION_ITERATOR_HPP