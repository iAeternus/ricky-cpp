/**
 * @brief 基于移动策略的迭代器封装
 * @author Ricky
 * @date 2024/12/2
 * @version 1.0
 */
#ifndef STRATEGY_DRIVEN_ITERATOR_HPP
#define STRATEGY_DRIVEN_ITERATOR_HPP

#include "Object.hpp"

namespace my::util {

/**
 * @brief 迭代器移动策略基类
 */
template <std::input_or_output_iterator I>
class IterStrategy : public Object<IterStrategy<I>> {
public:
    using iterator = I;
    using iterator_category = std::input_or_output_iterator;
    using value_type = std::remove_reference<decltype(*std::declval<iterator>())>;

    static iterator& next(iterator& iter) = delete;
    static iterator& prev(iterator& iter) = delete;
};

/**
 * @brief 自增自减策略
 */
template <std::bidirectional_iterator I>
class IncrDecrStrategy : public IterStrategy<I> {
public:
    using Self = IncrDecrStrategy<I>;
    using Super = IterStrategy<I>;

    using iterator = I;
    using iterator_category = std::bidirectional_iterator_tag;

    /**
     * @brief 移动到下一个元素，修改自身
     */
    static iterator& next(iterator& iter) {
        return ++iter;
    }

    /**
     * @brief 移动到上一个元素，修改自身
     */
    static iterator& prev(iterator& iter) {
        return --iter;
    }
};

/**
 * @brief 跳跃步长策略
 */
template <std::bidirectional_iterator I, usize N>
class StepJumpStrategy : public IterStrategy<I> {
public:
    using Self = StepJumpStrategy<I, N>;
    using Super = IterStrategy<I>;

    using iterator = I;
    using iterator_category = std::bidirectional_iterator;

    /**
     * @brief 跳跃步长，前向
     */
    static iterator& next(iterator& iter) {
        for (usize i = 0; i < N; ++i) {
            ++iter;
        }
        return iter;
    }

    /**
     * @brief 跳跃步长，后向
     */
    static iterator& prev(iterator& iter) {
        for (usize i = 0; i < N; ++i) {
            --iter;
        }
        return iter;
    }
};

/**
 * @brief 策略驱动的迭代器，根据策略定制不同的迭代行为
 */
template <typename Strategy>
class StrategyDrivenIterator : public Object<StrategyDrivenIterator<Strategy>> {
public:
    using Self = StrategyDrivenIterator<Strategy>;
    using Super = Object<Self>;

    using iterator = typename Strategy::iterator;
    using move_t = Strategy;
    using iterator_category = typename Strategy::iterator_category;
    using value_type = typename Strategy::value_type;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;

    StrategyDrivenIterator() :
            iter_() {}

    StrategyDrivenIterator(const iterator& iter) :
            iter_(iter) {}

    StrategyDrivenIterator(const Self& other) :
            iter_(other.iter_) {}

    StrategyDrivenIterator(Self&& other) noexcept :
            iter_(std::move(other.iter_)) {}

    Self& operator=(const Self& other) {
        if (this == &other) return *this;
        this->iter_ = other.iter_;
        return *this;
    }

    Self& operator=(Self&& other) noexcept {
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

    Self& operator++() {
        move_t::next(iter_);
        return *this;
    }

    Self operator++(i32) {
        iterator tmp(this);
        ++*this;
        return Self{tmp};
    }

    Self& operator--() {
        move_t::prev(iter_);
        return *this;
    }

    Self operator-(i32) {
        iterator tmp(this);
        --*this;
        return Self{tmp};
    }

    [[nodiscard]] bool __equals__(const Self& other) const {
        return iter_ == other.iter_;
    }

    bool operator==(const Self& other) const {
        return __equals__(other);
    }

    bool operator!=(const Self& other) const {
        return !__equals__(other);
    }

private:
    iterator iter_;
};

} // namespace my::util

#endif // STRATEGY_DRIVEN_ITERATOR_HPP