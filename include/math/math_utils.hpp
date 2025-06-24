/**
 * @brief 基本数学工具
 * @author Ricky
 * @date 2025/1/1
 * @version 1.0
 */
#ifndef MATH_UTILS_HPP
#define MATH_UTILS_HPP

#include "math_concepts.hpp"
#include "Exception.hpp"

#include <cmath>

namespace my::math {

static constexpr f64 PI = 3.14159265358979323846;
static constexpr f64 E = 2.71828182845904523536;
static constexpr f64 GOLDEN_RATIO = 0.61803398874989484820; // 黄金分割比
static constexpr i32 MOD = 1e9 + 7;                         // i32范围内的素数模数

template <typename T>
fn min_(const T& a, const T& b)->const T& {
    return b < a ? b : a;
}

template <typename T>
fn max_(const T& a, const T& b)->const T& {
    return a < b ? b : a;
}

/**
 * @brief 计算两个整数的最大公约数，O(log2(min(a, b)))
 */
template <IntegerType T>
fn gcd(const T& a, const T& b)->T {
    return b > 0 ? gcd(b, a % b) : a;
}

/**
 * @brief 计算两个整数的最小公倍数，O(log2(min(a, b)))
 */
template <IntegerType T>
fn lcm(const T& a, const T& b)->T {
    return a / gcd(a, b) * b;
}

/**
 * @brief 判断素数，O(sqrt(n))
 */
template <IntegerType T>
fn isprime(const T& num)->bool {
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
fn mul(const T& a, const T& b, T p = MOD)->T {
    T c = a * b - static_cast<T>(1.0L * a * b / p) * p;
    c %= p;
    if (c < 0) {
        c += p;
    }
    return c;
}

/**
 * @brief 计算 a^n
 */
template <IntegerType T>
fn pow(T a, T n)->T {
    T res{1};
    for (; n; n >>= 1, a *= a) {
        if (n & 1) {
            res *= a;
        }
    }
    return res;
}

template <IntegerType T>
fn pow2(T n)->T {
    return pow(T(2), n);
}

template <IntegerType T>
fn pow10(T n)->T {
    return pow(T(10), n);
}

/**
 * @brief 计算 (a^n) % p
 */
template <IntegerType T>
fn pow_mod(T a, T n, T p = MOD)->T {
    T res{1};
    for (; n; n >>= 1, a = mul(a, a, p)) {
        if (n & 1) {
            res = mul(res, a, p);
        }
    }
    return res;
}

#define EPS 1e-8 // 浮点数误差阈值

/**
 * @brief 在给定误差阈值下比较两个浮点数
 * @return a>b返回1，a<b返回-1，a=b返回0
 */
template <FloatingPointType T>
fn fcmp(const T& a, const T& b, T eps = EPS)->cmp_t {
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
fn is_pos(const T& num, T eps = EPS)->bool {
    return fcmp(num, 0.0, eps) > 0;
}

/**
 * @brief 在给定误差下判断浮点数是否为负
 * @return true=是 false=否
 */
template <FloatingPointType T>
fn is_neg(const T& num, T eps = EPS)->bool {
    return fcmp(num, 0.0, eps) < 0;
}

/**
 * @brief 在给定误差下判断浮点数是否为0
 * @return true=是 false=否
 */
template <FloatingPointType T>
fn is_zero(const T& num, T eps = EPS)->bool {
    return fcmp(num, 0.0, eps) == 0;
}

/**
 * @brief 在给定误差下判断浮点数是否为1
 * @return true=是 false=否
 */
template <FloatingPointType T>
fn is_one(const T& num, T eps = EPS)->bool {
    return fcmp(num, 1.0, eps) == 0;
}

/**
 * @brief 倒数
 */
fn reciprocal(f64 num)->f64 {
    if (is_zero(num)) {
        throw arithmetic_exception("/ by zero");
    }
    return 1.0 / num;
}

/**
 * @brief 处理 -0 问题
 */
template <FloatingPointType T>
fn correct_float(const T& num)->T {
    if (is_zero(num)) {
        return 0.0;
    }
    return num;
}

/**
 * @brief 相加并校验是否溢出，若溢出抛出RuntimeError
 */
fn add_exact(i64 x, i64 y)->i64 {
    i64 r = x + y;
    // HD 2-12 Overflow iff both arguments have the opposite sign of the result
    if (((x ^ r) & (y ^ r)) < 0) {
        throw overflow_exception("i64 overflow");
    }
    return r;
}

/**
 * @brief 相减并校验是否溢出，若溢出抛出RuntimeError
 */
fn sub_exact(i64 x, i64 y)->i64 {
    i64 r = x - y;
    // HD 2-12 Overflow iff the arguments have different signs and
    // the sign of the result is different from the sign of x
    if (((x ^ y) & (x ^ r)) < 0) {
        throw overflow_exception("i64 overflow");
    }
    return r;
}

/**
 * @brief 相乘并校验是否溢出，若溢出抛出RuntimeError
 */
fn mul_exact(i32 x, i32 y)->i32 {
    i64 r = i64(x) * i64(y);
    if (i32(r) != r) {
        throw overflow_exception("i32 overflow");
    }
    return i32(r);
}

fn mul_exact(i64 x, i64 y)->i64 {
    i64 r = x * y;
    i64 ax = std::abs(x);
    i64 ay = std::abs(y);
    if (((ax | ay) >> 31 != 0)) {
        // Some bits greater than 2^31 that might cause overflow
        // Check the result using the divide operator
        // and check for the special case of I64_MIN * -1
        if (((y != 0) && (r / y != x)) || (x == I64_MIN && y == -1)) {
            throw overflow_exception("i64 overflow");
        }
    }
    return r;
}

} // namespace my::math

#endif // MATH_UTILS_HPP