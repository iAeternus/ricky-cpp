#ifndef TEST_MATRIX_HPP
#define TEST_MATRIX_HPP

#include "ricky_test.hpp"
#include "Matrix.hpp"

namespace my::test::test_matrix {

auto should_construct = []() {
    math::Matrix m{3, 4, 1};
    math::Matrix m2 = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9},
    };
    math::Matrix m3;

    Assertions::assertEquals(3LL, m.rows());
    Assertions::assertEquals(4LL, m.cols());
    Assertions::assertEquals("[[1,1,1,1],[1,1,1,1],[1,1,1,1]]"_cs, m.__str__());

    Assertions::assertEquals(3LL, m2.rows());
    Assertions::assertEquals(3LL, m2.cols());
    Assertions::assertEquals("[[1,2,3],[4,5,6],[7,8,9]]"_cs, m2.__str__());

    Assertions::assertEquals(1LL, m3.rows());
    Assertions::assertEquals(1LL, m3.cols());
    Assertions::assertEquals("[[0]]"_cs, m3.__str__());
};

auto should_at = []() {
    math::Matrix m = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}};
    Assertions::assertEquals(0.0, m.at(0, 0));
    Assertions::assertEquals(m[0][0], m.at(0, 0));
    Assertions::assertEquals(8.0, m.at(2, 2));
    Assertions::assertEquals(m[2][2], m.at(2, 2));
};

auto should_get_mat = []() {
    // Given
    math::Matrix m = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}};

    // When
    auto res = m.getMat(1, 1, 2, 2);
    auto res2 = m.getMat(0, 0, -1, -1);

    // Then
    Assertions::assertEquals("[[4,5],[7,8]]"_cs, res.__str__());
    Assertions::assertEquals("[[0,1,2],[3,4,5],[6,7,8]]"_cs, res2.__str__());
};

auto should_fill = []() {
    // Given
    math::Matrix m = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}};

    // When
    m.fill(1);

    // Then
    Assertions::assertEquals("[[1,1,1],[1,1,1],[1,1,1]]"_cs, m.__str__());
};

auto should_add = []() {
    // Given
    math::Matrix m = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}};
    math::Matrix m2{3, 3, 1};

    // When
    auto res = m + m2;
    m += m2;

    // Then
    Assertions::assertEquals("[[1,2,3],[4,5,6],[7,8,9]]"_cs, res.__str__());
    Assertions::assertEquals(res, m);
};

auto should_subtract = []() {
    // Given
    math::Matrix m = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    math::Matrix m2{3, 3, 1};

    // When
    auto res = m - m2;
    m -= m2;

    // Then
    Assertions::assertEquals("[[0,1,2],[3,4,5],[6,7,8]]"_cs, res.__str__());
    Assertions::assertEquals(res, m);
};

auto should_multiply = []() {
    // Given
    math::Matrix m = {{1, 2}, {3, 4}};
    math::Matrix m2 = {{5, 6}, {7, 8}};

    // When
    auto res = m * m2;
    m *= m2;

    // Then
    Assertions::assertEquals("[[19,22],[43,50]]"_cs, res.__str__());
    Assertions::assertEquals(res, m);
};

auto should_dot = []() {
    // Given
    f64 n = 2;
    math::Matrix m = {{1, 2}, {3, 4}};
    math::Matrix m2 = {{5, 6}, {7, 8}};

    // When
    auto res = m.dot(m2);
    auto res2 = m.dot(n);

    // Then
    Assertions::assertEquals("[[5,12],[21,32]]"_cs, res.__str__());
    Assertions::assertEquals("[[2,4],[6,8]]"_cs, res2.__str__());
};

auto should_transpose = []() {
    // Given
    math::Matrix m = {{1, 2, 3}, {4, 5, 6}};

    // When
    auto res = m.T();

    // Then
    Assertions::assertEquals("[[1,4],[2,5],[3,6]]"_cs, res.__str__());
};

auto should_calculate_inverse = []() {
    // Given
    math::Matrix m = {{3, 0, 2}, {2, 0, -2}, {0, 1, 1}};

    // When
    auto res = m.inv();

    // Then
    Assertions::assertEquals("[[0.2,0.2,0],[-0.2,0.3,1],[0.2,-0.3,0]]"_cs, res.__str__());
};

auto should_lu_decomposition = []() {
    // Given
    math::Matrix m = {{1, 5, -3}, {-2, -7, 3}, {4, 9, 6}};

    // When
    auto[L, U] = m.LU();

    // Then
    Assertions::assertEquals("[[1,0,0],[-2,1,0],[4,-3.66667,1]]"_cs, L.__str__());
    Assertions::assertEquals("[[1,5,-3],[0,3,-3],[0,0,7]]"_cs, U.__str__());
};

void test_matrix() {
    UnitTestGroup group{"test_matrix"};

    group.addTest("should_construct", should_construct);
    group.addTest("should_at", should_at);
    group.addTest("should_get_mat", should_get_mat);
    group.addTest("should_fill", should_fill);
    group.addTest("should_add", should_add);
    group.addTest("should_subtract", should_subtract);
    group.addTest("should_multiply", should_multiply);
    group.addTest("should_dot", should_dot);
    group.addTest("should_transpose", should_transpose);
    group.addTest("should_calculate_inverse", should_calculate_inverse);
    group.addTest("should_lu_decomposition", should_lu_decomposition);

    group.startAll();
}

} // namespace my::test::test_matrix

#endif // TEST_MATRIX_HPP