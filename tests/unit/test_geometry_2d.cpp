#include "test_geometry_2d.hpp"
#include "geometry_2d.hpp"
#include "ricky_test.hpp"

namespace my::test::test_geometry_2d {

void should_calc_dot() {
    // Given
    math::Vector2 a{1, 1};
    math::Vector2 b{-1, 1};

    // When
    auto res = math::dot(a, b);

    // Then
    Assertions::assertEquals(0, res);
}

void should_calc_the_cos_of_vec() {
    // Given
    math::Vector2 a{1, 1};
    math::Vector2 b{0, 0};

    // When
    auto res = math::cos(a, b);

    // Then
    Assertions::assertEquals(1.0 / sqrt(2), res);
}

void should_calc_the_angle_between_two_vectors() {
    // Given
    math::Vector2 a{1, 0}, b{0, 1};

    // When
    auto res = math::angle(a, b);

    // Then
    Assertions::assertEquals(math::PI / 2, res);
}

void should_calc_cross() {
    // Given
    math::Vector2 a{1, 0}, b{0, 1};

    // When
    auto res = math::cross(a, b);

    // Then
    Assertions::assertEquals(1, res);
}

void should_calc_area_of_triangle() {
    // Given
    math::Point2 a{0, 0}, b{1, 0}, c{0, 1};

    // When
    auto res = math::area(a, b, c);

    // Then
    Assertions::assertEquals(0.5, res);
}

void should_calc_line_intersection() {
    // Given
    math::Line2 a{math::Point2{0, 1}, math::Point2{1, 0}};
    math::Line2 b{math::Point2{1, 0}, math::Point2{0, 1}};

    // When
    auto res = math::line_intersection(a, b);

    // Then
    Assertions::assertEquals(math::Point2{1, 1}, res);
}

void should_calc_distance() {
    // Given
    math::Point2 p{0, 0}, a{1, 0}, b{1, 1};

    // When
    auto res = math::distance(p, a, b);

    // Then
    Assertions::assertEquals(1, res);
}

void should_calc_distance_to_seg() {
    // Given
    math::Point2 p{0, 0}, p2{0, 2}, p3{0, -1};
    math::Point2 a{1, 0}, b{1, 1};

    // When
    auto res = math::distance_to_seg(p, a, b);
    auto res2 = math::distance_to_seg(p2, a, b);
    auto res3 = math::distance_to_seg(p3, a, b);

    // Then
    Assertions::assertEquals(1, res);
    Assertions::assertEquals(std::sqrt(2), res2);
    Assertions::assertEquals(std::sqrt(2), res3);
}

void should_calc_projection() {
    // Given
    math::Point2 p{0, 2}, p2{0, -1};
    math::Point2 a{1, 0}, b{1, 1};

    // When
    auto res = math::projection(p, a, b);
    auto res2 = math::projection(p2, a, b);

    // Then
    Assertions::assertEquals(math::Point2{1, 2}, res);
    Assertions::assertEquals(math::Point2{1, -1}, res2);
}

GROUP_NAME("test_geometry_2d")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(should_calc_dot),
    UNIT_TEST_ITEM(should_calc_the_cos_of_vec),
    UNIT_TEST_ITEM(should_calc_the_angle_between_two_vectors),
    UNIT_TEST_ITEM(should_calc_cross),
    UNIT_TEST_ITEM(should_calc_area_of_triangle),
    UNIT_TEST_ITEM(should_calc_line_intersection),
    UNIT_TEST_ITEM(should_calc_distance),
    UNIT_TEST_ITEM(should_calc_distance_to_seg),
    UNIT_TEST_ITEM(should_calc_projection))

} // namespace my::test::test_geometry_2d