/**
 * @brief 类型萃取工具
 * @author Ricky
 * @date 2024/11/24
 * @version 1.0
 */
#ifndef RICKY_TRAITS_HPP
#define RICKY_TRAITS_HPP

#include "ricky.hpp"

#include <type_traits>

namespace my {

// 判断Args的所有类型是否都与T相同
template <typename... Args>
struct IsSame : std::false_type {};

template <typename T1, typename... Args>
struct IsSame<T1, Args...> : std::bool_constant<(IsSame<T1, Args>::value || ...)> {};

template <typename T1, typename Arg>
struct IsSame<T1, Arg> : std::bool_constant<std::is_same_v<std::decay_t<T1>, std::decay_t<Arg>>> {};

template <typename T, typename... Args>
constexpr bool is_same = IsSame<T, Args...>::value;

// 判断T是否是Args的子类
template <typename... Args>
struct IsInstance : std::false_type {};

template <typename T1, typename... Args>
struct IsInstance<T1, Args...> : std::bool_constant<(IsInstance<T1, Args>::value || ...)> {};

template <typename T1, typename Arg>
struct IsInstance<T1, Arg> : std::bool_constant<std::is_base_of_v<Arg, T1> || is_same<T1, Arg>> {};

template <typename T, typename... Args>
constexpr bool is_instance = IsInstance<T, Args...>::value;

// 逻辑运算符
template <bool... B>
constexpr bool And = (B && ...);

template <bool... B>
constexpr bool Or = (B || ...);

template <bool B>
constexpr bool Not = !B;

// 用于判断是否为void，如果是void则返回void，否则返回T&
template <typename T>
struct VoidOrRef : std::type_identity<T&> {};

template <>
struct VoidOrRef<void> : std::type_identity<void> {};

template <typename T>
using void_or_ref_t = typename VoidOrRef<T>::type;

// 可迭代对象对内部迭代器类型萃取
template <typename T>
struct inner_iterator_type : std::type_identity<decltype(std::declval<T>().begin())> {};

template <typename T>
struct inner_iterator_type<T*> : std::type_identity<T*> {};

template <typename T>
struct inner_iterator_type<T[]> : std::type_identity<T*> {};

template <typename T, std::size_t N>
struct inner_iterator_type<T[N]> : std::type_identity<T*> {};

template <typename T>
requires hasattr(T, Iterator) struct inner_iterator_type<T> : std::type_identity<typename T::Iterator> {};

template <typename T>
using traits_inner_iterator_t = typename inner_iterator_type<T>::type;

/**
 * @brief 判断是否为合法的dtype
 */
template <typename dtype>
struct is_valid_dtype {
    static constexpr bool value = std::is_default_constructible<dtype>::value && std::is_nothrow_copy_constructible<dtype>::value && std::is_nothrow_move_constructible<dtype>::value && std::is_nothrow_copy_assignable<dtype>::value && std::is_nothrow_move_assignable<dtype>::value && std::is_nothrow_destructible<dtype>::value && !std::is_void<dtype>::value && !std::is_pointer<dtype>::value && !std::is_array<dtype>::value && !std::is_union<dtype>::value && !std::is_function<dtype>::value && !std::is_abstract<dtype>::value;
};

template <typename dtype>
constexpr bool is_valid_dtype_v = is_valid_dtype<dtype>::value;

}; // namespace my

#endif // RICKY_TRAITS_HPP