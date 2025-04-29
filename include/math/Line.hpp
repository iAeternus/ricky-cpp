/**
 * @brief 直线，点向式表示
 * @author Ricky
 * @date 2025/4/25
 * @version 1.0
 */
#ifndef LINE_HPP
#define LINE_HPP

#include "Vector2.hpp"

namespace my::math {

class Line : public Object<Line> {
public:
    using self = Line;

    Line(const Point2& p, const Vector2& s) :
            p_(p), s_(s), ang_(std::atan2(s.y(), s.x())) {}

    fn p() const { return p_; }
    fn s() const { return s_; }
    fn ang() const { return ang_; }

    cmp_t __cmp__(const self& other) const {
        return fcmp(ang_, other.ang_);
    }

private:
    Point2 p_;
    Vector2 s_;
    f64 ang_;
};

} // namespace my::math

#endif // LINE_HPP