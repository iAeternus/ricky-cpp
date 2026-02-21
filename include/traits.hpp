/**
 * @brief rust-like 常用 trait 定义，面向 concept
 * @author Ricky
 * @date 2026/2/16
 * @version 1.0
 */
#ifndef TRAITS_HPP
#define TRAITS_HPP

#include "my_types.hpp"

#include <string>
#include <concepts>

namespace my {

enum class Ordering : i8 {
    Less = -1,
    Equal = 0,
    Great = 1,
};

template <typename T>
concept Display = requires(const T& t) {
    { display_fmt(t) } -> std::convertible_to<std::string>;
};

} // namespace my

#endif // TRAITS_HPP