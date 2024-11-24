/**
 * @brief 类型萃取工具
 * @author Ricky
 * @date 2024/11/24
 * @version 1.0
 */
#ifndef RICKY_TRAITS_HPP
#define RICKY_TRAITS_HPP

#include <type_traits>

namespace ricky {

    // 判断Args的所有类型是否都与T相同
    template<typename... Args>
    struct IsSame : std::false_type {};

    template<typename T1, typename... Args>
    struct IsSame<T1, Args...> : std::bool_constant<(IsSame<T1, Args>::value || ...)> {};

    template<typename T1, typename Arg>
    struct IsSame<T1, Arg> : std::bool_constant<std::is_same_v<std::decay_t<T1>, std::decay_t<Arg>>> {};

    template<typename T, typename... Args>
    constexpr bool is_same = IsSame<T, Args...>::value;

    // 判断T是否是Args的子类
    template<typename... Args>
    struct IsInstance : std::false_type {};

    template<typename T1, typename... Args>
    struct IsInstance<T1, Args...> : std::bool_constant<(IsInstance<T1, Args>::value || ...)> {};

    template<typename T1, typename Arg>
    struct IsInstance<T1, Arg> : std::bool_constant<std::is_base_of_v<Arg, T1> || is_same<T1, Arg>> {};

    template<typename T, typename... Args>
    constexpr bool is_instance = IsInstance<T, Args...>::value;

    // TODO

};

#endif // RICKY_TRAITS_HPP