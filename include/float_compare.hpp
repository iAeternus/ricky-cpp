/**
 * @brief 浮点数比较工具
 * @author Ricky
 * @date 2025/6/5
 * @version 1.0
 *
 * 从 math/math_utils.hpp 中提取，供 test/assertions.hpp 使用，
 * 避免 test 模块依赖 math 模块。
 */
#ifndef FLOAT_COMPARE_HPP
#define FLOAT_COMPARE_HPP

#include "my_concepts.hpp"

namespace my {

#define FCMP_EPS 1e-8

/**
 * @brief 在给定误差阈值下比较两个浮点数
 * @return a>b返回1，a<b返回-1，a=b返回0
 */
template <FloatingPointType T>
auto fcmp(const T& a, const T& b, T eps = FCMP_EPS) -> cmp_t {
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
 */
template <FloatingPointType T>
auto is_pos(const T& num, T eps = FCMP_EPS) -> bool {
    return fcmp(num, 0.0, eps) > 0;
}

/**
 * @brief 在给定误差下判断浮点数是否为负
 */
template <FloatingPointType T>
auto is_neg(const T& num, T eps = FCMP_EPS) -> bool {
    return fcmp(num, 0.0, eps) < 0;
}

/**
 * @brief 在给定误差下判断浮点数是否为0
 */
template <FloatingPointType T>
auto is_zero(const T& num, T eps = FCMP_EPS) -> bool {
    return fcmp(num, 0.0, eps) == 0;
}

/**
 * @brief 在给定误差下判断浮点数是否为1
 */
template <FloatingPointType T>
auto is_one(const T& num, T eps = FCMP_EPS) -> bool {
    return fcmp(num, 1.0, eps) == 0;
}

/**
 * @brief 处理 -0 问题
 */
template <FloatingPointType T>
auto correct_float(const T& num) -> T {
    if (is_zero(num)) {
        return 0.0;
    }
    return num;
}

} // namespace my

#endif // FLOAT_COMPARE_HPP
