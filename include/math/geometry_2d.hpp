/**
 * @brief 2维计算几何库
 * @author Ricky
 * @date 2025/4/25
 * @version 1.0
 */
#ifndef GEOMETRY_2D_HPP
#define GEOMETRY_2D_HPP

#include "Vector2.hpp"
#include "Line2.hpp"

namespace my::math {

/**
 * @brief 计算两向量点积
 */
auto dot(const Vector2& a, const Vector2& b) -> f64 {
    return a.x() * b.x() + a.y() * b.y();
}

/**
 * @brief 计算两向量夹角余弦
 */
auto cos(const Vector2& a, const Vector2& b) -> f64 {
    return dot(a, b) / a.length() / b.length();
}

/**
 * @brief 计算两向量夹角，单位：弧度
 */
auto angle(const Vector2& a, const Vector2& b) -> f64 {
    return std::acos(cos(a, b));
}

/**
 * @brief 计算两向量叉积
 */
auto cross(const Vector2& a, const Vector2& b) -> f64 {
    return a.x() * b.y() - a.y() * b.x();
}

/**
 * @brief 计算三角形有向面积 A = 1/2 * (u x v)
 */
auto area(const Point2& a, const Point2& b, const Point2& c) -> f64 {
    return cross(b - a, c - a) / 2.0;
}

/**
 * @brief 计算两直线交点
 */
auto line_intersection(const Line2& a, const Line2& b) -> Point2 {
    auto u = a.p() - b.p();
    auto t = cross(b.s(), u) / cross(a.s(), b.s());
    return a.p() + a.s() * t;
}

/**
 * @brief 计算点p到直线AB距离
 */
auto distance(const Point2& p, const Point2& a, const Point2& b) -> f64 {
    auto v1 = b - a, v2 = p - a;
    return std::fabs(cross(v1, v2) / v1.length());
}

/**
 * @brief 计算点p到线段AB距离
 */
auto distance_to_seg(const Point2& p, const Point2& a, const Point2& b) -> f64 {
    if (a.__equals__(b)) {
        return (a - p).length();
    }

    auto v1 = b - a, v2 = p - a, v3 = p - b;
    if (is_neg(dot(v1, v2))) {
        return v2.length();
    } else if (is_pos(dot(v1, v3))) {
        return v3.length();
    } else {
        return distance(p, a, b);
    }
}

/**
 * @brief 计算点p在直线AB上的投影点
 */
auto projection(const Point2& p, const Point2& a, const Point2& b) -> Point2 {
    auto v = b - a;
    return a + v * (dot(v, p - a) / dot(v, v));
}

/**
 * @brief 判断点p是否在线段AB上
 */
auto is_point_on_seg(const Point2& p, const Point2& a, const Point2& b) -> bool {
    return is_zero(cross(a - p, b - p)) && is_neg(dot(a - p, b - p));
}

} // namespace my::math

#endif // GEOMETRY_2D_HPP