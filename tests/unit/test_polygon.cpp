#include "test_polygon.hpp"
#include "polygon.hpp"
#include "ricky_test.hpp"

namespace my::test::test_polygon {

void should_calc_area() {
    // Given
    math::Polygon p{{{0, 0}, {1, 0}, {1, 1}, {0, 1}}};

    // When
    auto res = p.area();

    // Then
    Assertions::assertEquals(1, res);
}

void should_judge_point_in_polygon() {
    // Given
    math::Polygon p{{{0, 0}, {1, 0}, {1, 1}, {0, 1}}};
    math::Point2 a{0.5, 0.5}, b{2, 2};

    // When
    auto res = p.is_included(a);
    auto res2 = p.is_included(b);

    // Then
    Assertions::assertTrue(res);
    Assertions::assertFalse(res2);
}

GROUP_NAME("test_polygon")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(should_calc_area))

} // namespace my::test::test_polygon