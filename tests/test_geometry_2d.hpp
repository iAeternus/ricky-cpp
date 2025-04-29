#ifndef TEST_GEOMETRY_2D_HPP
#define TEST_GEOMETRY_2D_HPP

#include "ricky_test.hpp"
#include "geometry_2d.hpp"

namespace my::test::test_geometry_2d {

auto should_calc_dot = []() {
    // Given
    math::Vector2 a{1, 1};
    math::Vector2 b{-1, 1};

    // When
    auto res = math::dot(a, b);

    // Then
    Assertions::assertEquals(0, res);
};

auto should_calc_the_cos_of_vec = []() {
    // Given
    math::Vector2 a{1, 1};
    math::Vector2 b{0, 0};
    
    // When
    auto res = math::cos(a, b);

    // Then
    Assertions::assertEquals(1.0 / sqrt(2), res);
};

void test_geometry_2d() {
    UnitTestGroup group{"test_geometry_2d"};

    group.addTest("should_calc_dot", should_calc_dot);
    group.addTest("should_calc_the_cos_of_vec", should_calc_the_cos_of_vec);

    group.startAll();
}

} // namespace my::test::test_geometry_2d

#endif // TEST_GEOMETRY_2D_HPP