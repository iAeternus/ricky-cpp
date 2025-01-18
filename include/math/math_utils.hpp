/**
 * @brief 基本数学工具
 * @author Ricky
 * @date 2025/1/1
 * @version 1.0
 */
#ifndef MATH_UTILS_HPP
#define MATH_UTILS_HPP

#include "math_concepts.hpp"

#include <cmath>
#include <cfloat>

namespace my::math {

static constexpr f64 PI = 3.14159265358979323846;
static constexpr f64 E = 2.71828182845904523536;
static constexpr f64 GOLDEN_RATIO = 0.61803398874989484820; // 黄金分割比
static constexpr i32 MOD = 1e9 + 7;                         // 素数模数

/**
 * @brief 计算两个整数的最大公约数，O(log2(min(a, b)))
 */
template <IntegerType T>
def gcd(const T& a, const T& b)->T {
    return b > 0 ? gcd(b, a % b) : a;
}

/**
 * @brief 计算两个整数的最小公倍数，O(log2(min(a, b)))
 */
template <IntegerType T>
def lcm(const T& a, const T& b)->T {
    return a / gcd(a, b) * b;
}

/**
 * @brief 判断素数，O(sqrt(n))
 */
template <IntegerType T>
def isprime(const T& num)->bool {
    if (num < 2) {
        return false;
    }

    T sqrtNum = std::sqrt(num);
    for (T i = 2; i <= sqrtNum; ++i) {
        if (num % i == 0) {
            return false;
        }
    }
    return true;
}

/**
 * @brief 计算 (a * b) % p
 */
template <IntegerType T>
def mul(const T& a, const T& b, T p = MOD)->T {
    T c = a * b - static_cast<T>(1.0L * a * b / p) * p;
    c %= p;
    if (c < 0) {
        c += p;
    }
    return c;
}

/**
 * @brief 计算 (a^n) % p
 */
template <IntegerType T>
def pow(T a, T n, T p = MOD)->T {
    T res{1};
    for (; n; n /= 2, a = mul(a, a, p)) {
        if (n % 2) {
            res = mul(res, a, p);
        }
    }
    return res;
}

#define EPS DBL_EPSILON // 浮点数误差阈值

/**
 * @brief 在给定误差阈值下比较两个浮点数
 * @return a>b返回1，a<b返回-1，a=b返回0
 */
template <FloatingPointType T>
def compare(const T& a, const T& b, T eps = EPS)->i32 {
    if (a - b >= eps) {
        return 1;
    } else if (b - a >= eps) {
        return -1;
    } else {
        return 0;
    }
}

/**
 * @brief 在给定误差下判断浮点数是否为正
 * @return true=是 false=否
 */
template <FloatingPointType T>
def isPositive(const T& num, T eps = EPS)->bool {
    return compare(num, 0.0, eps) > 0;
}

/**
 * @brief 在给定误差下判断浮点数是否为负
 * @return true=是 false=否
 */
template <FloatingPointType T>
def isNegative(const T& num, T eps = EPS)->bool {
    return compare(num, 0.0, eps) < 0;
}

/**
 * @brief 在给定误差下判断浮点数是否为0
 * @return true=是 false=否
 */
template <FloatingPointType T>
def isZero(const T& num, T eps = EPS)->bool {
    return compare(num, 0.0, eps) == 0;
}

/**
 * @brief 在给定误差下判断浮点数是否为1
 * @return true=是 false=否
 */
template <FloatingPointType T>
def isOne(const T& num, T eps = EPS)->bool {
    return compare(num, 1.0, eps) == 0;
}

/**
 * @brief 倒数
 */
def reciprocal(f64 num)->f64 {
    if (isZero(num)) {
        ValueError("Divided by 0.");
        return None<f64>;
    }
    return 1.0 / num;
}

/**
 * @brief 处理 -0 问题
 */
template <FloatingPointType T>
def correctFloat(const T& num)->T {
    if (std::signbit(num) && num == 0.0) {
        return 0.0;
    }
    return num;
}

} // namespace my::math

#endif // MATH_UTILS_HPP