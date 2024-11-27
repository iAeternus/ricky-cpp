/**
 * @brief 约束与概念
 * @author Ricky
 * @date 2024/11/27
 * @version 1.0
 */
#ifndef RICKY_CONCEPTS_HPP
#define RICKY_CONCEPTS_HPP

#include "ricky.hpp"
#include "ricky_traits.hpp"

#include <concepts>
#include <string>

namespace my {

// 可以隐式转换为const char*的类型约束概念
template <typename S>
concept ConvertibleToCstr = std::convertible_to<S, const char*>;

// 可输出的类型约束概念
template <typename T>
concept StdPrintable = std::is_pointer_v<T> || is_same<T, bool, char, char*, short, unsigned short, int, unsigned int, long, unsigned long, long long, unsigned long long, float, double, long double, nullptr_t, std::string>;

template <typename T>
concept MyPrintable = requires(T t) {
    { t.__str__() }
    ->ConvertibleToCstr;
};

template <typename T>
concept Printable = MyPrintable<T> || StdPrintable<T>;

// 可哈希的类型约束概念
template <typename T>
concept MyLikeHashable = requires(const T& one, const T& other) {
    { one.__hash__() }
    ->std::convertible_to<hash_t>;
    { one.__cmp__(other) }
    ->std::convertible_to<cmp_t>;
};

template <typename T>
concept StdHashable = requires(const T& one, const T& other) {
    { std::hash<T>()(one) }
    ->std::convertible_to<hash_t>;
    { one == other }
    ->std::convertible_to<bool>;
};

template <typename T>
concept Hashable = MyLikeHashable<T> || StdHashable<T>;

// 可迭代类型约束概念
template <typename T>
concept Iterable = requires(T obj) {
    {obj.begin()};
    {obj.end()};
    {obj.size()};
};

} // namespace my

#endif // RICKY_CONCEPTS_HPP