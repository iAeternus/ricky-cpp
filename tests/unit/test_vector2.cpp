#include "test_vector2.hpp"
#include "vector2.hpp"
#include "ricky_test.hpp"

namespace my::test::test_vector2 {

void it_works() {
    // Given
    math::Vector2 v{1, -1};
    math::Vector2 v2{1, 1};

    // When
    auto res = v + v2;
    auto res2 = v - v2;
    auto res3 = v * 2;
    auto res4 = 2 * v;

    auto res5 = v;
    v += v2;
    v -= v2;
    v *= 2;
    v *= 0.5;

    Assertions::assertEquals(math::Vector2{2, 0}, res);
    Assertions::assertEquals(math::Vector2{0, -2}, res2);
    Assertions::assertEquals(math::Vector2{2, -2}, res3);
    Assertions::assertEquals(math::Vector2{2, -2}, res4);
    Assertions::assertEquals(v, res5);
}

void should_calc_pa() {
    // Given
    math::Vector2 v{1, 0};
    math::Vector2 v2{1, 1};
    math::Vector2 v3{0, 1};
    math::Vector2 v4{-1, 0};

    // When
    auto res = v.pa();
    auto res2 = v2.pa();
    auto res3 = v3.pa();
    auto res4 = v4.pa();

    // Then
    Assertions::assertEquals(0, res);
    Assertions::assertEquals(math::PI / 4, res2);
    Assertions::assertEquals(math::PI / 2, res3);
    Assertions::assertEquals(math::PI, res4);
}

void should_calc_length() {
    // Given
    math::Vector2 v{1, 0};
    math::Vector2 v2{0, 0};

    // When
    auto res = v.length();
    auto res2 = v2.length();

    // Then
    Assertions::assertEquals(1, res);
    Assertions::assertEquals(0, res2);
}

void should_rotate() {
    // Given
    math::Vector2 v{1, 0};
    f64 rad = math::PI / 2;

    // When
    auto res = v.rotate(rad);

    // Then
    Assertions::assertEquals(math::Vector2{0, 1}, res);
}

void should_calc_unit() {
    // Given
    math::Vector2 v{1, 1};

    // When
    auto res = v.unit();

    // Then
    Assertions::assertEquals(math::Vector2{1.0 / sqrt(2), 1.0 / sqrt(2)}, res);
}

void should_calc_norm() {
    // Given
    math::Vector2 v{1, 1};

    // When
    auto res = v.norm();

    // Then
    Assertions::assertEquals(math::Vector2{-1.0 / sqrt(2), 1.0 / sqrt(2)}, res);
}

GROUP_NAME("test_vector2")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(it_works),
    UNIT_TEST_ITEM(should_calc_pa),
    UNIT_TEST_ITEM(should_calc_length),
    UNIT_TEST_ITEM(should_rotate),
    UNIT_TEST_ITEM(should_calc_unit))

} // namespace my::test::test_vector2