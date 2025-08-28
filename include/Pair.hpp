/**
 * @brief 有序二元组
 * @author Ricky
 * @date 2025/1/11
 * @version 1.0
 */
#ifndef PAIR_HPP
#define PAIR_HPP

#include "Exception.hpp"

#include <tuple>

namespace my {

/**
 * @class Pair
 * @brief 有序二元组
 * @tparam S 第一个元素类型
 * @tparam T 第二个元素类型
 */
template <typename S, typename T>
class Pair : public Object<Pair<S, T>> {
public:
    using Self = Pair<S, T>;

    Pair() noexcept = default;

    Pair(S first, T second) :
            first_(std::move(first)), second_(std::move(second)) {}

    Pair(Self&& other) noexcept :
            first_(std::move(other.first_)), second_(std::move(other.second_)) {}

    Pair(const Self& other) :
            first_(other.first_), second_(other.second_) {}

    Self& operator=(Self&& other) noexcept {
        if (this == &other) return *this;

        first_ = std::move(other.first_);
        second_ = std::move(other.second_);
        return *this;
    }

    Self& operator=(const Self& other) {
        if (this == &other) return *this;

        first_ = other.first_;
        second_ = other.second_;
        return *this;
    }

    S& first() {
        return first_;
    }

    const S& first() const {
        return first_;
    }

    T& second() {
        return second_;
    }

    const T& second() const {
        return second_;
    }

    template <std::size_t I>
    decltype(auto) get() & {
        if constexpr (I == 0) {
            return first_;
        } else if constexpr (I == 1) {
            return second_;
        }
    }

    template <std::size_t I>
    decltype(auto) get() const& {
        if constexpr (I == 0) {
            return first_;
        } else if constexpr (I == 1) {
            return second_;
        }
    }

    template <std::size_t I>
    decltype(auto) get() && {
        if constexpr (I == 0) {
            return std::move(first_);
        } else if constexpr (I == 1) {
            return std::move(second_);
        }
    }

    [[nodiscard]] cmp_t __cmp__(const Self& other) const {
        if constexpr (Comparable<S> && Comparable<T>) {
            auto cmp = first_.__cmp__(other.first_);
            if (cmp != 0) return cmp;
            return second_.__cmp__(other.second_);
        } else if constexpr (Subtractble<S> && Subtractble<T>) {
            auto cmp = first_ - other.first_;
            if (cmp != 0) return cmp;
            return second_ - other.second_;
        } else {
            throw runtime_exception("The comparison logic has not been implemented.");
        }
    }

    [[nodiscard]] CString __str__() const {
        std::stringstream stream;
        stream << '('; // TODO
        if constexpr (is_same<S, CString/*, String*/, std::string>) {
            stream << '\"' << first_ << '\"';
        } else {
            stream << first_;
        }
        stream << ", ";
        if constexpr (is_same<T, CString/*, String*/, std::string>) {
            stream << '\"' << second_ << '\"';
        } else {
            stream << second_;
        }
        stream << ')';
        return CString{stream.str()};
    }

private:
    S first_{};
    T second_{};
};

} // namespace my

/**
 * @brief 结构化绑定支持
 */
template <typename S, typename T>
struct std::tuple_size<my::Pair<S, T>>
        : integral_constant<size_t, 2> {};

template <size_t I, typename S, typename T>
struct std::tuple_element<I, my::Pair<S, T>> {
    static_assert(I < 2, "Index out of bounds for Pair");
    using type = conditional_t<I == 0, S, T>;
};

namespace my {

// 基于索引的get函数
template <std::size_t I, typename S, typename T>
auto& get(Pair<S, T>& p) {
    static_assert(I < 2, "Pair index out of range");
    if constexpr (I == 0) {
        return p.first();
    } else {
        return p.second();
    }
}

template <std::size_t I, typename S, typename T>
const auto& get(const Pair<S, T>& p) {
    static_assert(I < 2, "Pair index out of range");
    if constexpr (I == 0) {
        return p.first();
    } else {
        return p.second();
    }
}

template <std::size_t I, typename S, typename T>
auto&& get(Pair<S, T>&& p) {
    static_assert(I < 2, "Pair index out of range");
    if constexpr (I == 0) {
        return std::move(p.first());
    } else {
        return std::move(p.second());
    }
}

} // namespace my

#endif // PAIR_HPP