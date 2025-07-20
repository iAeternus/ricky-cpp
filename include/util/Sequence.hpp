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
     * @brief 下标访问
     */
    value_t& at(usize idx) {
        return static_cast<D*>(this)->at(idx);
    }

    /**
     * @brief 下标访问
     */
    const value_t& at(usize idx) const {
        return static_cast<const D*>(this)->at(idx);
    }

    /**
     * @brief 获取序列的大小
     * @return 返回序列的大小
     */
    usize size() const noexcept {
        return static_cast<const D*>(this)->size();
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

    /**
     * @brief 检查是否包含指定值
     * @param v 需要检查的值
     */
    template <typename V>
    bool contains(const V& v) const {
        return find_itr(v) != end();
    }

    /**
     * @brief 查找指定值的索引
     * @param v 需要查找的值
     * @return 返回值在序列中的索引，如果不存在则返回 npos
     */
    template <typename V>
    usize find(const V& v) const {
        for (usize i = 0, siz = size(); i < siz; ++i) {
            if (at(i) == v) {
                return i;
            }
        }
        return npos;
    }

    /**
     * @brief 查找第一个匹配值的位置
     * @param v 需要查找的值
     * @return 返回值在序列中的第一个匹配位置索引，如果不存在则返回 npos
     */
    template <typename V>
    usize find_first_of(const V& v) const {
        for (usize i = 0, siz = size(); i < siz; ++i) {
            if (at(i) == v) {
                return i;
            }
        }
        return npos;
    }

    /**
     * @brief 查找最后一个匹配值的位置
     * @param v 需要查找的值
     * @return 返回值在序列中的最后一个匹配位置索引，如果不存在则返回 npos
     */
    template <typename V>
    usize find_last_of(const V& v) const {
        for (i32 i = size() - 1; i >= 0; --i) {
            if (at(i) == v) {
                return i;
            }
        }
        return npos;
    }

    /**
     * @brief 查找指定值的迭代器
     * @param v 需要查找的值
     * @return 返回指向值的迭代器，如果不存在则返回 end()
     */
    template <typename V>
    iterator find_itr(const V& v) {
        auto it = begin(), end_ = end();
        while (it != end_) {
            if (*it == v) {
                return it;
            }
            ++it;
        }
        return end_;
    }

    /**
     * @brief 查找指定值的常量迭代器
     * @param v 需要查找的值
     * @return 返回指向值的常量迭代器，如果不存在则返回 end()
     */
    template <typename V>
    const_iterator find_itr(const V& v) const {
        auto it = begin(), end_ = end();
        while (it != end_) {
            if (*it == v) {
                return it;
            }
            ++it;
        }
        return end_;
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
};

} // namespace my::util

#endif // SEQUENCE_HPP