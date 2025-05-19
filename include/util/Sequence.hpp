/**
 * @brief 序列
 * @author Ricky
 * @date 2024/12/1
 * @version 1.0
 */
#ifndef SEQUENCE_HPP
#define SEQUENCE_HPP

#include "IndexIterator.hpp"

namespace my::util {

/**
 * 需要子类实现 size() 和 at()
 * @tparam D 实现类类型
 * @tparam T 元素类型
 */
template <typename D, typename T>
class Sequence : public Object<Sequence<D, T>> {
public:
    using value_t = T;
    using Self = Sequence<D, value_t>;
    using Super = Object<Sequence<D, value_t>>;

    using iterator = IndexIterator<false, Self, value_t>;
    using const_iterator = IndexIterator<true, Self, value_t>;

    value_t& at(usize idx) {
        return static_cast<D*>(this)->at(idx);
    }

    const value_t& at(usize idx) const {
        return static_cast<const D*>(this)->at(idx);
    }

    usize size() const {
        return static_cast<const D*>(this)->size();
    }

    [[nodiscard]] cmp_t __cmp__(const D& other) const {
        auto m_it = begin(), m_end = end();
        auto o_it = other.begin(), o_end = other.end();
        while (m_it != m_end && o_it != o_end) {
            if (*m_it < *o_it) return -1;
            if (*m_it > *o_it) return 1;
            ++m_it, ++o_it;
        }
        if (m_it != m_end) return 1;
        if (o_it != o_end) return -1;
        return 0;
    }

    [[nodiscard]] bool __equals__(const D& other) const {
        if (size() != other.size()) {
            return false;
        }
        return static_cast<const D*>(this)->__cmp__(other) == 0;
    }

    iterator begin() {
        return iterator(this, 0);
    }

    iterator end() {
        return iterator(this, size());
    }

    const_iterator begin() const {
        return const_iterator(this, 0);
    }

    const_iterator end() const {
        return const_iterator(this, size());
    }

    /**
     * @brief 中括号下标访问，允许负数下标
     * @note 负数下标代表倒序下标
     */
    value_t& operator[](isize idx) {
        return at(neg_index(idx, size()));
    }

    /**
     * @brief 中括号下标访问，允许负数下标
     * @note 负数下标代表倒序下标
     */
    const value_t& operator[](isize idx) const {
        return at(neg_index(idx, size()));
    }

    bool contains(const value_t& v) const {
        return find_itr(v) != end();
    }

    usize find(const value_t& v) const {
        for (usize i = 0, siz = size(); i < siz; ++i) {
            if (at(i) == v) {
                return i;
            }
        }
        return npos;
    }

    iterator find_itr(const value_t& v) {
        auto it = begin(), end_ = end();
        while (it != end_) {
            if (*it == v) {
                return it;
            }
            ++it;
        }
        return end_;
    }

    const_iterator find_itr(const value_t& v) const {
        auto it = begin(), end_ = end();
        while (it != end_) {
            if (*it == v) {
                return it;
            }
            ++it;
        }
        return end_;
    }
};

} // namespace my::util

#endif // SEQUENCE_HPP