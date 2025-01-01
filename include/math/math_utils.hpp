/**
 * @brief 基本数学工具
 * @author Ricky
 * @date 2025/1/1
 * @version 1.0
 */
#ifndef MATH_UTILS_HPP
#define MATH_UTILS_HPP

#include "ricky.hpp"

#include <cmath>

namespace my::math {

static constexpr double PI = 3.14159265358979323846;
static constexpr double E = 2.71828182845904523536;
static constexpr double GOLDEN_RATIO = 0.61803398874989484820; // 黄金分割比

/**
 * @brief 检查类型是否为无符号整数类型的约束
 */
template <typename T>
concept UnsignedIntegerType = std::is_integral_v<T> && !std::is_signed_v<T>;

/**
 * @brief 检查类型是否为有符号整数类型的约束
 */
template <typename T>
concept SignedIntegerType = std::is_integral_v<T>&& std::is_signed_v<T>;

/**
 * @brief 检查类型是否为整数类型的约束
 */
template <typename T>
concept IntegerType = UnsignedIntegerType<T> || SignedIntegerType<T>;

/**
 * @brief 检查类型是否为浮点类型的约束
 */
template <typename T>
concept FloatingPointType = std::is_floating_point_v<T>;

/**
 * @brief 检查类型是否为数值类型的约束
 */
template <typename T>
concept NumberType = IntegerType<T> || FloatingPointType<T>;

/**
 * @brief 计算两个整数的最大公约数
 */
template <IntegerType T>
def gcd(T a, T b)->T {
    return b > 0 ? gcd(b, a % b) : a;
}

/**
 * @brief 计算两个整数的最小公倍数
 */
template <IntegerType T>
def lcm(T a, T b)->T {
    return a / gcd(a, b) * b;
}

} // namespace my::math

#endif // MATH_UTILS_HPP