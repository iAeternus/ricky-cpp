/**
 * @brief 多边形
 * @author Ricky
 * @date 2025/5/1
 * @version 1.0
 */
#ifndef POLYGON_HPP
#define POLYGON_HPP

#include "geometry_2d.hpp"
#include "Vec.hpp"

namespace my::math {

class Polygon : public Object<Polygon> {
public:
    using Self = Polygon;

    Polygon(const util::Vec<Point2>& points) :
            points_(points) {}

    f64 area() const {
        f64 ans = 0.0;
        auto n = points_.size() - 1;
        for (int i = 1; i < n; ++i) {
            ans += math::cross(points_[i] - points_[0], points_[i + 1] - points_[0]);
        }
        return ans / 2.0;
    }

    bool is_included(const Point2& p) const {
        i32 wn = 0, n = points_.size();
        for (int i = 0; i < n; ++i) {
            if (is_point_on_seg(p, points_[i], points_[(i + 1) % n])) return true;
            if (p == points_[i]) return true;

            auto k = fcmp(cross(points_[(i + 1) % n] - points_[i], p - points_[i]), 0.0);
            auto d1 = fcmp(points_[i].y() - p.y(), 0.0), d2 = fcmp(points_[(i + 1) % n].y() - p.y(), 0.0);
            if (k > 0 && d1 <= 0 && d2 > 0) {
                ++wn;
            }
            if (k < 0 && d2 <= 0 && d1 > 0) {
                --wn;
            }
        }
        return wn != 0;
    }

private:
    util::Vec<Point2> points_;
};

} // namespace my::math

#endif // POLYGON_HPP