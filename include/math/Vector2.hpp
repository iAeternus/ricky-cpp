/**
 * @brief 2维向量
 * @author Ricky
 * @date 2025/4/25
 * @version 1.0
 */
#ifndef VECTOR2_HPP
#define VECTOR2_HPP

#include "math_utils.hpp"

namespace my::math {

class Vector2 : public Object<Vector2> {
public:
    using Self = Vector2;

    Vector2(f64 x, f64 y) :
            x_(x), y_(y) {}

    Vector2(const Self& other) :
            x_(other.x_), y_(other.y_) {}

    Self& operator=(const Self& other) {
        if (*this == other) return *this;
        this->x_ = other.x_;
        this->y_ = other.y_;
        return *this;
    }

    auto x() const {
        return x_;
    }

    auto y() const {
        return y_;
    }

    auto x(f64 x) -> Self& {
        this->x_ = x;
        return *this;
    }

    auto y(f64 y) -> Self& {
        this->y_ = y;
        return *this;
    }

    /**
     * @brief 计算极角
     */
    f64 pa() const {
        return std::atan2(y_, x_);
    }

    /**
     * @brief 计算模长
     */
    f64 length() const {
        return std::sqrt(x_ * x_ + y_ * y_);
    }

    /**
     * @brief 逆时针旋转 rad 弧度
     */
    Self rotate(double rad) const {
        return Self{x_ * std::cos(rad) - y_ * std::sin(rad), x_ * std::sin(rad) + y_ * std::cos(rad)};
    }

    /**
     * @brief 计算单位向量
     */
    Self unit() const {
        f64 len = length();
        if (is_zero(len)) {
            return Self{0, 0};
        }
        return Self{x_ / len, y_ / len};
    }

    /**
     * @brief 计算单位法向量
     */
    Self norm() const {
        f64 len = length();
        if (is_zero(len)) {
            return Self{0, 0};
        }
        return Self{-y_ / len, x_ / len};
    }

    friend Self operator+(const Self& a, const Self& b) {
        return Self{a.x_ + b.x_, a.y_ + b.y_};
    }

    Self& operator+=(const Self& other) {
        this->x_ += other.x_;
        this->y_ += other.y_;
        return *this;
    }

    friend Self operator-(const Self& a, const Self& b) {
        return Self{a.x_ - b.x_, a.y_ - b.y_};
    }

    Self& operator-=(const Self& other) {
        this->x_ -= other.x_;
        this->y_ -= other.y_;
        return *this;
    }

    friend Self operator*(f64 p, const Self& v) {
        return Self{v.x_ * p, v.y_ * p};
    }

    friend Self operator*(const Self& v, f64 p) {
        return Self{v.x_ * p, v.y_ * p};
    }

    Self& operator*=(f64 p) {
        this->x_ *= p;
        this->y_ *= p;
        return *this;
    }

    [[nodiscard]] CString __str__() const {
        std::stringstream stream;
        stream << '(' << x_ << ',' << y_ << ')';
        return CString{stream.str()};
    }

    [[nodiscard]] cmp_t __cmp__(const Self& other) const {
        auto x_cmp = fcmp(x_, other.x_);
        if (x_cmp != 0) return x_cmp;
        return fcmp(y_, other.y_);
    }

    [[nodiscard]] bool __equals__(const Self& other) const {
        return fcmp(x_, other.x_) == 0 && fcmp(y_, other.y_) == 0;
    }

private:
    f64 x_, y_;
};

using Point2 = Vector2;

} // namespace my::math

#endif // VECTOR2_HPP