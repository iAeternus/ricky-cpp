#ifndef MATH_CONCEPTS_HPP
#define MATH_CONCEPTS_HPP

#include <type_traits>
#include "my_traits.hpp"

namespace my::math {

/**
 * @brief 检查类型是否为无符号整数类型的约束
 */
template <typename T>
concept UnsignedIntegerType = std::is_integral_v<RawType<T>> && !std::is_signed_v<RawType<T>>;

/**
 * @brief 检查类型是否为有符号整数类型的约束
 */
template <typename T>
concept SignedIntegerType = std::is_integral_v<RawType<T>> && std::is_signed_v<RawType<T>>;

/**
 * @brief 检查类型是否为整数类型的约束
 */
template <typename T>
concept IntegerType = UnsignedIntegerType<T> || SignedIntegerType<T>;

/**
 * @brief 检查类型是否为浮点类型的约束
 */
template <typename T>
concept FloatingPointType = std::is_floating_point_v<RawType<T>>;

/**
 * @brief 检查类型是否为数值类型的约束
 */
template <typename T>
concept NumericType = IntegerType<T> || FloatingPointType<T>;

} // namespace my::math

#endif // MATH_CONCEPTS_HPP