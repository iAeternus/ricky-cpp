/**
 * @brief 2维向量
 * @author Ricky
 * @date 2025/4/25
 * @version 1.0
 */
#ifndef VECTOR2_HPP
#define VECTOR2_HPP

#include "Object.hpp"
#include "math_utils.hpp"

namespace my::math {

class Vector2 : public Object<Vector2> {
public:
    using self = Vector2;

    Vector2(f64 x, f64 y) :
            x_(x), y_(y) {}

    Vector2(const self& other) :
            x_(other.x_), y_(other.y_) {}

    self& operator=(const self& other) {
        if (*this == other) return *this;
        this->x_ = other.x_;
        this->y_ = other.y_;
        return *this;
    }

    fn x() const {
        return x_;
    }

    fn y() const {
        return y_;
    }

    fn x(f64 x)->self& {
        this->x_ = x;
        return *this;
    }

    fn y(f64 y)->self& {
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
    self rotate(double rad) const {
        return self{x_ * std::cos(rad) - y_ * std::sin(rad), x_ * std::sin(rad) + y_ * std::cos(rad)};
    }

    /**
     * @brief 计算单位向量
     */
    self unit() const {
        f64 len = length();
        if (is_zero(len)) {
            return self{0, 0};
        }
        return self{x_ / len, y_ / len};
    }

    /**
     * @brief 计算单位法向量
     */
    self norm() const {
        f64 len = length();
        if (is_zero(len)) {
            return self{0, 0};
        }
        return self{-y_ / len, x_ / len};
    }

    friend self operator+(const self& a, const self& b) {
        return self{a.x_ + b.x_, a.y_ + b.y_};
    }

    self& operator+=(const self& other) {
        this->x_ += other.x_;
        this->y_ += other.y_;
        return *this;
    }

    friend self operator-(const self& a, const self& b) {
        return self{a.x_ - b.x_, a.y_ - b.y_};
    }

    self& operator-=(const self& other) {
        this->x_ -= other.x_;
        this->y_ -= other.y_;
        return *this;
    }

    friend self operator*(f64 p, const self& v) {
        return self{v.x_ * p, v.y_ * p};
    }

    friend self operator*(const self& v, f64 p) {
        return self{v.x_ * p, v.y_ * p};
    }

    self& operator*=(f64 p) {
        this->x_ *= p;
        this->y_ *= p;
        return *this;
    }

    CString __str__() const {
        std::stringstream stream;
        stream << '(' << x_ << ',' << y_ << ')';
        return CString{stream.str()};
    }

    cmp_t __cmp__(const self& other) const {
        auto x_cmp = fcmp(x_, other.x_);
        if (x_cmp != 0) return x_cmp;
        return fcmp(y_, other.y_);
    }

    bool __equals__(const self& other) const {
        return fcmp(x_, other.x_) == 0 && fcmp(y_, other.y_) == 0;
    }

private:
    f64 x_, y_;
};

using Point2 = Vector2;

} // namespace my::math

#endif // VECTOR2_HPP