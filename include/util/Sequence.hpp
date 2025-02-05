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
 */
template <typename Derived, typename T>
class Sequence : public Object<Sequence<Derived, T>> {
public:
    using self = Sequence<Derived, T>;
    using super = Object<Sequence<Derived, T>>;

public:
    using value_t = T;
    using iterator = IndexIterator<false, self, value_t>;
    using const_iterator = IndexIterator<true, self, value_t>;

    value_t& at(c_size index) {
        return static_cast<Derived*>(this)->at(index);
    }

    const value_t& at(c_size index) const {
        return static_cast<const Derived*>(this)->at(index);
    }

    c_size size() const {
        return static_cast<const Derived*>(this)->size();
    }

    cmp_t __cmp__(const Derived& other) const {
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

    bool __equals__(const Derived& other) const {
        if (size() != other.size()) {
            return false;
        }
        return static_cast<const Derived*>(this)->__cmp__(other) == 0;
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

    value_t& operator[](c_size index) {
        return at(neg_index(index, size()));
    }

    const value_t& operator[](c_size index) const {
        return at(neg_index(index, size()));
    }

    bool contains(const value_t& v) const {
        return find_it(v) != end();
    }

    c_size find(const value_t& v) const {
        for (c_size i = 0, siz = size(); i < siz; ++i) {
            if (at(i) == v) {
                return i;
            }
        }
        return -1;
    }

    iterator find_it(const value_t& v) {
        auto it = begin(), end_ = end();
        while (it != end_) {
            if (*it == v) {
                return it;
            }
            ++it;
        }
        return end_;
    }

    const_iterator find_it(const value_t& v) const {
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