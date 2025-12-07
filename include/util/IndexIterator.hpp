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

/**
 * @class IndexIterator
 * @brief 索引容器迭代器 TODO Alloc
 * @tparam IsConst 是否为const迭代器
 * @tparam C 容器类行
 * @tparam V 元素类型
 */
template <bool IsConst, typename C, typename V, typename Alloc = mem::Allocator<V>>
class IndexIterator : public Object<IndexIterator<IsConst, C, V, Alloc>> {
    template <bool, typename, typename, typename>
    friend class IndexIterator;

public:
    using Super = Object<IndexIterator>;
    using Self = IndexIterator;
    using allocator_type = Alloc;

    using container_t = std::conditional_t<IsConst, const C, C>;
    using iterator_category = std::random_access_iterator_tag;
    using value_type = std::conditional_t<IsConst, const V, V>;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;

    IndexIterator(container_t* container = nullptr, const usize index = 0) :
            container_(container), index_(index) {}

    template <bool OtherConst>
        requires(IsConst && !OtherConst)
    IndexIterator(const IndexIterator<OtherConst, C, V>& other) :
            container_(other.container_), index_(other.index_) {}

    IndexIterator(const Self& other) :
            IndexIterator(other.container_, other.index_) {}

    template <bool OtherConst>
        requires(IsConst && !OtherConst)
    Self& operator=(const IndexIterator<OtherConst, C, V>& other) {
        this->container_ = other.container_;
        this->index_ = other.index_;
        return *this;
    }

    /**
     * @brief 解引用
     */
    reference operator*() const noexcept
        requires(!IsConst || std::is_const_v<container_t>)
    {
        return container_->operator[](index_);
    }

    pointer operator->() const noexcept
        requires(!IsConst || std::is_const_v<container_t>)
    {
        return &container_->operator[](index_);
    }

    /**
     * @brief 前缀递增/递减
     */
    Self& operator++() noexcept {
        ++index_;
        return *this;
    }

    Self& operator--() noexcept {
        --index_;
        return *this;
    }

    /**
     * @breif 后缀递增/递减
     */
    Self operator++(i32) noexcept {
        Self tmp(*this);
        ++*this;
        return tmp;
    }

    Self operator--(i32) noexcept {
        Self tmp(*this);
        --*this;
        return tmp;
    }

    /**
     * @brief 随机访问
     */
    Self operator+(difference_type n) const noexcept {
        return Self(container_, index_ + n);
    }

    Self operator-(difference_type n) const noexcept {
        return Self(container_, index_ - n);
    }

    Self& operator+=(difference_type n) noexcept {
        index_ += n;
        return *this;
    }

    Self& operator-=(difference_type n) noexcept {
        index_ -= n;
        return *this;
    }

    difference_type operator-(const Self& other) const noexcept {
        return static_cast<difference_type>(index_ - other.index_);
    }

    /**
     * @brief 下标访问
     */
    reference operator[](difference_type n) const noexcept {
        return container_->operator[](index_ + n);
    }

    [[nodiscard]] bool __equals__(const Self& other) const {
        return container_ == other.container_ && index_ == other.index_;
    }

    [[nodiscard]] cmp_t __cmp__(const Self& other) const {
        return this->index_ - other.index_;
    }

private:
    container_t* container_; // 容器
    usize index_;            // 索引
};

/**
 * @brief 推导指南 TODO Alloc
 */
template <typename C, typename V, typename Alloc = mem::Allocator<V>>
IndexIterator(C*, usize) -> IndexIterator<false, C, V, Alloc>;

template <typename C, typename V, typename Alloc = mem::Allocator<V>>
IndexIterator(const C*, usize) -> IndexIterator<true, C, V, Alloc>;

} // namespace my::util

#endif // INDEX_ITERATOR_HPP
