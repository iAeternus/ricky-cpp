/**
 * @brief 约束与概念
 * @author Ricky
 * @date 2024/11/27
 * @version 2.1
 */
#ifndef MY_CONCEPTS_HPP
#define MY_CONCEPTS_HPP

#include "my_traits.hpp"

#include <concepts>
#include <string>

namespace my {

/**
 * @brief 可隐式转换为 const char* 的类型
 */
template <typename S>
concept ConvertibleToCstr = std::convertible_to<S, const char*>;

/**
 * @brief 可直接用标准库输出的类型
 */
template <typename T>
concept StdPrintable = std::is_pointer_v<T> ||
                       is_same<T, bool, char, char*, short, unsigned short, int, unsigned int, long, unsigned long, long long,
                               unsigned long long, float, double, long double, std::nullptr_t, const char*, std::string>;

/**
 * @brief 支持 to_string 协议的类型
 */
template <typename T>
concept MyPrintable = requires(const T& t) {
    t.to_string();
};

/**
 * @brief 可打印类型
 */
template <typename T>
concept Printable = MyPrintable<T> || StdPrintable<T>;

/**
 * @brief 支持自定义哈希协议的类型
 */
template <typename T>
concept MyLikeHashable = requires(const T& one, const T& other) {
    { one.hash() } -> std::convertible_to<hash_t>;
    { one.cmp(other) } -> std::convertible_to<cmp_t>;
};

/**
 * @brief 支持标准库哈希的类型
 */
template <typename T>
concept StdHashable = requires(const T& one, const T& other) {
    { std::hash<T>()(one) } -> std::convertible_to<hash_t>;
    { one == other } -> std::convertible_to<bool>;
};

/**
 * @brief 可哈希类型
 */
template <typename T>
concept Hashable = MyLikeHashable<T> || StdHashable<T>;

/**
 * @brief 可迭代类型
 */
template <typename T>
concept Iterable = requires(T obj) {
    { obj.begin() };
    { obj.end() };
    { obj.len() };
};

/**
 * @brief 支持比较协议的类型
 */
template <typename T>
concept Comparable = requires(const T& one, const T& other) {
    { one.cmp(other) } -> std::convertible_to<cmp_t>;
};

/**
 * @brief 可做差的类型
 */
template <typename T>
concept Subtractble = requires(const T& one, const T& other) {
    { one - other } -> std::convertible_to<T>;
};

/**
 * @brief 可排序类型
 */
template <typename T>
concept Sortable = Comparable<T> || Subtractble<T>;

/**
 * @brief 可作为键的类型
 */
template <typename T>
concept KeyType = Hashable<T> || Sortable<T>;

/**
 * @brief 可断言类型
 */
template <typename T>
concept Assertable = Comparable<T> && MyPrintable<T>;

/**
 * @brief 非类类型
 */
template <typename T>
concept NonClassType = !std::is_class_v<T>;

/**
 * @brief 有效数据类型
 */
template <typename T>
concept DType = is_valid_dtype_v<T>;

/**
 * @brief 分配器感知类型
 */
template <typename C>
concept AllocatorAware = requires {
    typename C::allocator_type;
};

} // namespace my

#endif // MY_CONCEPTS_HPP
