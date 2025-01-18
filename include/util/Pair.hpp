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

namespace my::util {

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

} // namespace my::util

// template <typename S, typename T>
// struct std::tuple_size<my::util::Pair<S, T>> : public std::integral_constant<std::size_t, 2> {};

// template <std::size_t I, typename S, typename T>
// struct std::tuple_element<I, my::util::Pair<S, T>>;

// template <typename S, typename T>
// struct std::tuple_element<0, my::util::Pair<S, T>> {
//     using type = S;
// };

// template <typename S, typename T>
// struct std::tuple_element<1, my::util::Pair<S, T>> {
//     using type = T;
// };

// // template<std::size_t I, typename S, typename T>
// // def get(my::util::Pair<S, T>&);

// // template <typename S, typename T>
// // def get<0, S, T>(my::util::Pair<S, T>& pair)->S& {
// //     return pair.first();
// // }

// // template <typename S, typename T>
// // def get<1, S, T>(my::util::Pair<S, T>& pair)->T& {
// //     return pair.second();
// // }

// template<std::size_t I, typename S, typename T>
// def std::get(my::util::Pair<S, T>& pair) {
//     static_assert(I < 2);
//     if constexpr (I == 0) {
//         return pair.first();
//     } else {
//         return pair.second();
//     }
// }

#endif // PAIR_HPP