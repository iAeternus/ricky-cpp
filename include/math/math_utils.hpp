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

namespace my::math {

static constexpr double PI = 3.14159265358979323846;
static constexpr double E = 2.71828182845904523536;
static constexpr double GOLDEN_RATIO = 0.61803398874989484820; // 黄金分割比
static constexpr i32 MOD = 1e9 + 7;                            // 素数模数

/**
 * @brief 计算两个整数的最大公约数，O(log2(min(a, b)))
 */
template <IntegerType T>
def gcd(T a, T b)->T {
    return b > 0 ? gcd(b, a % b) : a;
}

/**
 * @brief 计算两个整数的最小公倍数，O(log2(min(a, b)))
 */
template <IntegerType T>
def lcm(T a, T b)->T {
    return a / gcd(a, b) * b;
}

/**
 * @brief 判断素数，O(sqrt(n))
 */
template <IntegerType T>
def isprime(T num)->bool {
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
def mul(T a, T b, T p = MOD)->T {
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

} // namespace my::math

#endif // MATH_UTILS_HPP