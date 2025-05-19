/**
 * @brief 直线，点向式表示
 * @author Ricky
 * @date 2025/4/25
 * @version 1.0
 */
#ifndef LINE2_HPP
#define LINE2_HPP

#include "Vector2.hpp"

namespace my::math {

class Line2 : public Object<Line2> {
public:
    using Self = Line2;

    Line2(const Point2& p, const Vector2& s) :
            p_(p), s_(s), ang_(std::atan2(s.y(), s.x())) {}

    fn p() const { return p_; }
    fn s() const { return s_; }
    fn ang() const { return ang_; }

    [[nodiscard]] cmp_t __cmp__(const Self& other) const {
        return fcmp(ang_, other.ang_);
    }

private:
    Point2 p_;
    Vector2 s_;
    f64 ang_;
};

} // namespace my::math

#endif // LINE_HPP