/**
 * @brief 2维计算几何库
 * @author Ricky
 * @date 2025/4/25
 * @version 1.0
 */
#ifndef GEOMETRY_2D_HPP
#define GEOMETRY_2D_HPP

#include "Vector2.hpp"

namespace my::math {

/**
 * @brief 计算两向量点积
 */
fn dot(const Vector2& a, const Vector2& b) -> f64 {
    return a.x() * b.x() + a.y() * b.y();
}

/**
 * @brief 计算两向量夹角余弦
 */
fn cos(const Vector2& a, const Vector2& b) -> f64 {
    return dot(a, b) / a.length() / b.length();
}

/**
 * @brief 计算两向量夹角，单位：弧度
 */
fn angle(const Vector2& a, const Vector2& b) -> f64 {
    return std::acos(cos(a, b));
}

/**
 * @brief 计算两向量叉积
 */
fn cross(const Vector2& a, const Vector2& b) -> f64 {
    return a.x() * b.y() - a.y() * b.x();
}

/**
 * @brief 计算三角形有向面积 A = 1/2 * (u x v)
 */
fn area(const Point2& a, const Point2& b, const Point2& c) -> f64 {
    return cross(b - a, c - a) / 2.0;
}

} // namespace my::math

#endif // GEOMETRY_2D_HPP