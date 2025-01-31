/**
 * @brief 有序二元组
 * @author Ricky
 * @date 2025/1/11
 * @version 1.0
 */
#ifndef PAIR_HPP
#define PAIR_HPP

#include "Object.hpp"

#include <tuple>

namespace my {

class String;

template <typename S, typename T>
class Pair : public Object<Pair<S, T>> {
    using self = Pair<S, T>;

public:
    Pair(S first, T second) :
            first_(std::move(first)), second_(std::move(second)) {}

    Pair(self&& other) noexcept :
            first_(std::move(other.first_)), second_(std::move(other.second_)) {}

    Pair(const self& other) :
            first_(other.first_), second_(other.second_) {}

    self& operator=(self&& other) noexcept {
        if (this == &other) return *this;

        first_ = std::move(other.first_);
        second_ = std::move(other.second_);
        return *this;
    }

    self& operator=(const self& other) {
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

    CString __str__() const {
        std::stringstream stream;
        stream << '(';
        if constexpr (is_same<S, CString, String, std::string>) {
            stream << '\"' << first_ << '\"';
        } else {
            stream << first_;
        }
        stream << ", ";
        if constexpr (is_same<T, CString, String, std::string>) {
            stream << '\"' << second_ << '\"';
        } else {
            stream << second_;
        }
        stream << ')';
        return CString{stream.str()};
    }

private:
    S first_;
    T second_;
};

} // namespace my

/**
 * @brief 结构化绑定支持
 */
namespace std {

template <typename S, typename T>
struct tuple_size<my::Pair<S, T>>
        : integral_constant<size_t, 2> {};

template <size_t I, typename S, typename T>
struct tuple_element<I, my::Pair<S, T>> {
    static_assert(I < 2, "Index out of bounds for Pair");
    using type = conditional_t<I == 0, S, T>;
};

} // namespace std

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