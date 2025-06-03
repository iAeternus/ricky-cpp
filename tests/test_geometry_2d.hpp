#ifndef TEST_GEOMETRY_2D_HPP
#define TEST_GEOMETRY_2D_HPP

#include "UnitTest.hpp"
#include "Assertions.hpp"
#include "geometry_2d.hpp"

namespace my::test::test_geometry_2d {

fn should_calc_dot = []() {
    // Given
    math::Vector2 a{1, 1};
    math::Vector2 b{-1, 1};

    // When
    auto res = math::dot(a, b);

    // Then
    Assertions::assertEquals(0, res);
};

fn should_calc_the_cos_of_vec = []() {
    // Given
    math::Vector2 a{1, 1};
    math::Vector2 b{0, 0};

    // When
    auto res = math::cos(a, b);

    // Then
    Assertions::assertEquals(1.0 / sqrt(2), res);
};

fn should_calc_the_angle_between_two_vectors = []() {
    // Given
    math::Vector2 a{1, 0}, b{0, 1};

    // When
    auto res = math::angle(a, b);

    // Then
    Assertions::assertEquals(math::PI / 2, res);
};

fn should_calc_cross = []() {
    // Given
    math::Vector2 a{1, 0}, b{0, 1};

    // When
    auto res = math::cross(a, b);

    // Then
    Assertions::assertEquals(1, res);
};

fn should_calc_area_of_triangle = []() {
    // Given
    math::Point2 a{0, 0}, b{1, 0}, c{0, 1};

    // When
    auto res = math::area(a, b, c);

    // Then
    Assertions::assertEquals(0.5, res);
};

fn should_calc_line_intersection = []() {
    // Given
    math::Line2 a{math::Point2{0, 1}, math::Point2{1, 0}};
    math::Line2 b{math::Point2{1, 0}, math::Point2{0, 1}};

    // When
    auto res = math::line_intersection(a, b);

    // Then
    Assertions::assertEquals(math::Point2{1, 1}, res);
};

fn should_calc_distance = []() {
    // Given
    math::Point2 p{0, 0}, a{1, 0}, b{1, 1};

    // When
    auto res = math::distance(p, a, b);

    // Then
    Assertions::assertEquals(1, res);
};

fn should_calc_distance_to_seg = []() {
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
};

fn should_calc_projection = []() {
    // Given
    math::Point2 p{0, 2}, p2{0, -1};
    math::Point2 a{1, 0}, b{1, 1};

    // When
    auto res = math::projection(p, a, b);
    auto res2 = math::projection(p2, a, b);

    // Then
    Assertions::assertEquals(math::Point2{1, 2}, res);
    Assertions::assertEquals(math::Point2{1, -1}, res2);
};

fn test_geometry_2d() {
    UnitTestGroup group{"test_geometry_2d"};

    group.addTest("should_calc_dot", should_calc_dot);
    group.addTest("should_calc_the_cos_of_vec", should_calc_the_cos_of_vec);
    group.addTest("should_calc_the_angle_between_two_vectors", should_calc_the_angle_between_two_vectors);
    group.addTest("should_calc_cross", should_calc_cross);
    group.addTest("should_calc_area_of_triangle", should_calc_area_of_triangle);
    group.addTest("should_calc_line_intersection", should_calc_line_intersection);
    group.addTest("should_calc_distance", should_calc_distance);
    group.addTest("should_calc_distance_to_seg", should_calc_distance_to_seg);
    group.addTest("should_calc_projection", should_calc_projection);

    group.startAll();
}

} // namespace my::test::test_geometry_2d

#endif // TEST_GEOMETRY_2D_HPP