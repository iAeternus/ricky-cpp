#ifndef TEST_POLYGON_HPP
#define TEST_POLYGON_HPP

#include "ricky_test.hpp"
#include "Polygon.hpp"

namespace my::test::test_polygon {

auto should_calc_area = []() {
    // Given
    math::Polygon p{{{0, 0}, {1, 0}, {1, 1}, {0, 1}}};

    // When
    auto res = p.area();

    // Then
    Assertions::assertEquals(1, res);
};

auto should_judge_point_in_polygon = []() {
    // Given
    math::Polygon p{{{0, 0}, {1, 0}, {1, 1}, {0, 1}}};
    math::Point2 a{0.5, 0.5}, b{2, 2};

    // When
    auto res = p.is_included(a);
    auto res2 = p.is_included(b);

    // Then
    Assertions::assertTrue(res);
    Assertions::assertFalse(res2);
};

auto test_polygon() {
    UnitTestGroup group{"test_polygon"};

    group.addTest("should_calc_area", should_calc_area);

    group.startAll();
}

} // namespace my::test::test_polygon

#endif // TEST_POLYGON_HPP