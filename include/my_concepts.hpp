/**
 * @brief 约束与概念
 * @author Ricky
 * @date 2024/11/27
 * @version 1.0
 */
#ifndef MY_CONCEPTS_HPP
#define MY_CONCEPTS_HPP

#include "my_traits.hpp"

#include <concepts>
#include <string>

namespace my {

/**
 * @brief 可以隐式转换为const char*的类型
 */
template <typename S>
concept ConvertibleToCstr = std::convertible_to<S, const char*>;

/**
 * @brief 可使用标准库打印的类型
 */
template <typename T>
concept StdPrintable = std::is_pointer_v<T> || is_same<T, bool, char, char*, short, unsigned short, int, unsigned int, long, unsigned long, long long, unsigned long long, float, double, long double, std::nullptr_t, const char*, std::string>;

/**
 * @brief 可使用__str__打印的类型
 */
template <typename T>
concept MyPrintable = requires(T t) {
    { t.__str__() } -> ConvertibleToCstr;
};

/**
 * @brief 可打印的类型
 */
template <typename T>
concept Printable = MyPrintable<T> || StdPrintable<T>;

/**
 * @brief 可使用__hash__哈希的类型
 */
template <typename T>
concept MyLikeHashable = requires(const T& one, const T& other) {
    { one.__hash__() } -> std::convertible_to<hash_t>;
    { one.__cmp__(other) } -> std::convertible_to<cmp_t>;
};

/**
 * @brief 可使用标准库哈希的类型
 */
template <typename T>
concept StdHashable = requires(const T& one, const T& other) {
    { std::hash<T>()(one) } -> std::convertible_to<hash_t>;
    { one == other } -> std::convertible_to<bool>;
};

/**
 * @brief 可哈希的类型
 */
template <typename T>
concept Hashable = MyLikeHashable<T> || StdHashable<T>;

/**
 * @brief 可迭代的类型
 */
template <typename T>
concept Iterable = requires(T obj) {
    { obj.begin() };
    { obj.end() };
    { obj.len() };
};

/**
 * @brief 可使用__cmp__比较的类型
 */
template <typename T>
concept Comparable = requires(const T& one, const T& other) {
    { one.__cmp__(other) } -> std::convertible_to<cmp_t>;
};

/**
 * @brief 可使用作差比较的类型
 */
template <typename T>
concept Subtractble = requires(const T& one, const T& other) {
    { one - other } -> std::convertible_to<T>;
};

/**
 * @brief 可排序的类型
 */
template <typename T>
concept Sortable = Comparable<T> || Subtractble<T>;

/**
 * @brief 可作为键的类型
 */
template <typename T>
concept KeyType = Hashable<T> || Sortable<T>;

/**
 * @brief 可断言的类型
 */
template <typename T>
concept Assertable = Comparable<T> && MyPrintable<T>;

/**
 * @brief 非类类型
 */
template <typename T>
concept NonClassType = !std::is_class_v<T>;

/**
 * @brief Data Type
 */
template <typename T>
concept DType = is_valid_dtype_v<T>;

/// 分配器感知
template <typename C>
concept AllocatorAware = requires {
    typename C::allocator_type;
};

} // namespace my

#endif // MY_CONCEPTS_HPP