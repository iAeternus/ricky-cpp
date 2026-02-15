#include "unit/test_matrix.hpp"

#include "ricky_test.hpp"
#include "matrix.hpp"

#include "test/test_registry.hpp"

namespace my::test::test_matrix {

void should_construct() {
    math::Matrix<f64> m(3, 4, 1);
    math::Matrix<f64> m2 = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9},
    };
    math::Matrix<f64> m3;

    Assertions::assertEquals(3, m.rows());
    Assertions::assertEquals(4, m.cols());
    Assertions::assertEquals("[[1,1,1,1],[1,1,1,1],[1,1,1,1]]"_cs, m.__str__());

    Assertions::assertEquals(3, m2.rows());
    Assertions::assertEquals(3, m2.cols());
    Assertions::assertEquals("[[1,2,3],[4,5,6],[7,8,9]]"_cs, m2.__str__());

    Assertions::assertEquals(1, m3.rows());
    Assertions::assertEquals(1, m3.cols());
    Assertions::assertEquals("[[0]]"_cs, m3.__str__());
}

void should_at() {
    math::Matrix<f64> m = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}};
    Assertions::assertEquals(0.0, m.at(0, 0));
    Assertions::assertEquals(m[0][0], m.at(0, 0));
    Assertions::assertEquals(8.0, m.at(2, 2));
    Assertions::assertEquals(m[2][2], m.at(2, 2));
}

void should_fail_to_get_if_index_out_of_bounds() {
    // Given
    math::Matrix<f64> m = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}};

    // When & Then
    Assertions::assertThrows("Index [2, 3] out of bounds [0..3, 0..3]", [&]() {
        m.at(2, 3);
    });

    Assertions::assertThrows("Column index 3 out of bounds [0..3]", [&]() {
        m[2][3];
    });
}

void should_get_sub_mat() {
    // Given
    math::Matrix<f64> m = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}};

    // When
    auto res = m.sub_mat(1, 1, 2, 2);

    // Then
    Assertions::assertEquals(res.__str__(), res.to_matrix().__str__());
    Assertions::assertEquals("[[4,5],[7,8]]"_cs, res.to_matrix().__str__());
}

void should_fail_to_get_sub_mat_if_index_invalid() {
    // Given
    math::Matrix<f64> m = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}};

    // When & Then
    Assertions::assertThrows("cannot get submatrix [2..1] x [2..1] of a (3x3) matrix.", [&]() {
        m.sub_mat(2, 2, 1, 1);
    });

    Assertions::assertThrows("cannot get submatrix [0..3] x [0..3] of a (3x3) matrix.", [&]() {
        m.sub_mat(0, 0, 3, 3);
    });
}

void should_fill() {
    // Given
    math::Matrix<f64> m = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}};

    // When
    m.fill(1);

    // Then
    Assertions::assertEquals("[[1,1,1],[1,1,1],[1,1,1]]"_cs, m.__str__());
}

void should_add() {
    // Given
    math::Matrix<f64> m = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}};
    math::Matrix<f64> m2(3, 3, 1);

    // When
    auto res = m + m2;
    m += m2;

    // Then
    Assertions::assertEquals("[[1,2,3],[4,5,6],[7,8,9]]"_cs, res.__str__());
    Assertions::assertEquals(res, m);
}

void should_fail_to_add_if_matrix_not_match() {
    // Given
    math::Matrix<f64> m(3, 3);
    math::Matrix<f64> m2(3, 2);

    // When & Then
    Assertions::assertThrows("cannot add a (3x3) matrix and a (3x2) matrix.", [&]() {
        m + m2;
    });

    Assertions::assertThrows("cannot add a (3x3) matrix and a (3x2) matrix.", [&]() {
        m += m2;
    });
}

void should_subtract() {
    // Given
    math::Matrix<f64> m = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    math::Matrix<f64> m2(3, 3, 1);

    // When
    auto res = m - m2;
    m -= m2;

    // Then
    Assertions::assertEquals("[[0,1,2],[3,4,5],[6,7,8]]"_cs, res.__str__());
    Assertions::assertEquals(res, m);
}

void should_fail_to_subtract_if_matrix_not_match() {
    // Given
    math::Matrix<f64> m(3, 3);
    math::Matrix<f64> m2(3, 2);

    // When & Then
    Assertions::assertThrows("cannot substract a (3x3) matrix and a (3x2) matrix.", [&]() {
        m - m2;
    });

    Assertions::assertThrows("cannot substract a (3x3) matrix and a (3x2) matrix.", [&]() {
        m -= m2;
    });
}

void should_multiply() {
    // Given
    math::Matrix<f64> m = {{1, 2}, {3, 4}};
    math::Matrix<f64> m2 = {{5, 6}, {7, 8}};

    // When
    auto res = m * m2;
    m *= m2;

    // Then
    Assertions::assertEquals("[[19,22],[43,50]]"_cs, res.__str__());
    Assertions::assertEquals(res, m);
}

void should_fail_to_multiply_if_matrix_not_match() {
    // Given
    math::Matrix<f64> m(3, 2);
    math::Matrix<f64> m2(3, 2);

    // When & Then
    Assertions::assertThrows("cannot multiply a (3x2) matrix and a (3x2) matrix.", [&]() {
        m* m2;
    });

    Assertions::assertThrows("cannot multiply a (3x2) matrix and a (3x2) matrix.", [&]() {
        m *= m2;
    });
}

void should_dot() {
    // Given
    f64 n = 2;
    math::Matrix<f64> m = {{1, 2}, {3, 4}};
    math::Matrix<f64> m2 = {{5, 6}, {7, 8}};

    // When
    auto res = m.dot(m2);
    auto res2 = m.dot(n);

    // Then
    Assertions::assertEquals("[[5,12],[21,32]]"_cs, res.__str__());
    Assertions::assertEquals("[[2,4],[6,8]]"_cs, res2.__str__());
}

void should_fail_to_dot_if_matrix_not_match() {
    // Given
    math::Matrix<f64> m(3, 3);
    math::Matrix<f64> m2(3, 2);

    // When & Then
    Assertions::assertThrows("cannot dot a (3x3) matrix and a (3x2) matrix.", [&]() {
        m.dot(m2);
    });
}

void should_transpose() {
    // Given
    math::Matrix<f64> m = {{1, 2, 3}, {4, 5, 6}};

    // When
    auto res = m.t();

    // Then
    Assertions::assertEquals("[[1,4],[2,5],[3,6]]"_cs, res.__str__());
}

void should_calculate_inverse() {
    // Given
    math::Matrix<f64> m = {{3, 0, 2}, {2, 0, -2}, {0, 1, 1}};

    // When
    auto res = m.inv();

    // Then
    Assertions::assertEquals("[[0.2,0.2,0],[-0.2,0.3,1],[0.2,-0.3,0]]"_cs, res.__str__());
}

void should_fail_to_calc_inv_if_matrix_not_square() {
    // Given
    math::Matrix<f64> m(3, 2);

    // When & Then
    Assertions::assertThrows("only square matrices have inverse matrices.", [&]() {
        m.inv();
    });
}

void should_calculate_det() {
    // Given
    math::Matrix<f64> m = {{1, 1, -1, 2}, {-1, -1, -4, 1}, {2, 4, -6, 1}, {1, 2, 4, 2}};

    // When
    auto res = m.det();

    // Then
    Assertions::assertEquals(57.0, res);
}

void should_fail_to_calc_det_if_matrix_not_square() {
    // Given
    math::Matrix<f64> m(3, 2);

    // When & Then
    Assertions::assertThrows("only square matrices can have their determinants calculated.", [&]() {
        m.det();
    });
}

void should_calculate_rank() {
    // Given
    math::Matrix<f64> m = {{1, 2, 3}, {2, 4, 6}, {3, 6, 9}};

    // When
    auto res = m.rank();

    // Then
    Assertions::assertEquals(1, res);
}

void should_lu_decomposition() {
    // Given
    math::Matrix<f64> m = {{1, 5, -3}, {-2, -7, 3}, {4, 9, 6}};

    // When
    auto [L, U] = m.lu();

    // Then
    Assertions::assertEquals("[[1,0,0],[-2,1,0],[4,-3.66667,1]]"_cs, L.__str__());
    Assertions::assertEquals("[[1,5,-3],[0,3,-3],[0,0,7]]"_cs, U.__str__());
}

void should_fail_to_lu_decomposition_if_matrix_not_square() {
    // Given
    math::Matrix<f64> m(3, 2);

    // When & Then
    Assertions::assertThrows("only square matrices are LU decomposition.", [&]() {
        m.lu();
    });
}

void should_cmp() {
    // Given
    math::Matrix<f64> m = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    math::Matrix<f64> m2 = {{1, 2, 3}, {4, 5, 6}, {9, 8, 7}};
    math::Matrix<f64> m3 = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};

    // When
    auto res = m.__cmp__(m2);
    auto res2 = m.__cmp__(m3);

    // Then
    Assertions::assertEquals(-1, res);
    Assertions::assertEquals(0, res2);
}

void should_fail_to_cmp_if_matrix_not_match() {
    // Given
    math::Matrix<f64> m(3, 3);
    math::Matrix<f64> m2(3, 2);

    // When & Then
    Assertions::assertThrows("only matrices of the same dimension are comparable", [&]() {
        auto _ = m.__cmp__(m2);
    });
}

void test_matrix_view() {
    // Given
    math::Matrix<f64> m = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};

    // When
    auto v = m.sub_mat(1, 1, 2, 2);
    auto v2 = v.sub_view(1, 1, 1, 1);

    // Then
    Assertions::assertEquals("[[5,6],[8,9]]"_cs, v.__str__());
    Assertions::assertEquals("[[9]]"_cs, v2.__str__());
    Assertions::assertEquals(8, v.at(1, 0));
    Assertions::assertEquals(8, v.at_abs(2, 1));
}

void test_matrix() {
    UnitTestGroup group{"test_matrix"};

    group.addTest("should_construct", should_construct);
    group.addTest("should_at", should_at);
    group.addTest("should_fail_to_get_if_index_out_of_bounds", should_fail_to_get_if_index_out_of_bounds);
    group.addTest("should_get_sub_mat", should_get_sub_mat);
    group.addTest("should_fail_to_get_sub_mat_if_index_invalid", should_fail_to_get_sub_mat_if_index_invalid);
    group.addTest("should_fill", should_fill);
    group.addTest("should_add", should_add);
    group.addTest("should_fail_to_add_if_matrix_not_match", should_fail_to_add_if_matrix_not_match);
    group.addTest("should_subtract", should_subtract);
    group.addTest("should_fail_to_subtract_if_matrix_not_match", should_fail_to_subtract_if_matrix_not_match);
    group.addTest("should_multiply", should_multiply);
    group.addTest("should_fail_to_multiply_if_matrix_not_match", should_fail_to_multiply_if_matrix_not_match);
    group.addTest("should_dot", should_dot);
    group.addTest("should_fail_to_dot_if_matrix_not_match", should_fail_to_dot_if_matrix_not_match);
    group.addTest("should_transpose", should_transpose);
    group.addTest("should_calculate_inverse", should_calculate_inverse);
    group.addTest("should_fail_to_calc_inv_if_matrix_not_square", should_fail_to_calc_inv_if_matrix_not_square);
    group.addTest("should_calculate_det", should_calculate_det);
    group.addTest("should_fail_to_calc_det_if_matrix_not_square", should_fail_to_calc_det_if_matrix_not_square);
    group.addTest("should_calculate_rank", should_calculate_rank);
    group.addTest("should_lu_decomposition", should_lu_decomposition);
    group.addTest("should_fail_to_lu_decomposition_if_matrix_not_square", should_fail_to_lu_decomposition_if_matrix_not_square);
    group.addTest("should_cmp", should_cmp);
    group.addTest("should_fail_to_cmp_if_matrix_not_match", should_fail_to_cmp_if_matrix_not_match);
    group.addTest("test_matrix_view", test_matrix_view);

    group.startAll();
}

GROUP_NAME("test_matrix")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(should_construct),
    UNIT_TEST_ITEM(should_at),
    UNIT_TEST_ITEM(should_fail_to_get_if_index_out_of_bounds),
    UNIT_TEST_ITEM(should_get_sub_mat),
    UNIT_TEST_ITEM(should_fail_to_get_sub_mat_if_index_invalid),
    UNIT_TEST_ITEM(should_fill),
    UNIT_TEST_ITEM(should_add),
    UNIT_TEST_ITEM(should_fail_to_add_if_matrix_not_match),
    UNIT_TEST_ITEM(should_subtract),
    UNIT_TEST_ITEM(should_fail_to_subtract_if_matrix_not_match),
    UNIT_TEST_ITEM(should_multiply),
    UNIT_TEST_ITEM(should_fail_to_multiply_if_matrix_not_match),
    UNIT_TEST_ITEM(should_dot),
    UNIT_TEST_ITEM(should_fail_to_dot_if_matrix_not_match),
    UNIT_TEST_ITEM(should_transpose),
    UNIT_TEST_ITEM(should_calculate_inverse),
    UNIT_TEST_ITEM(should_fail_to_calc_inv_if_matrix_not_square),
    UNIT_TEST_ITEM(should_calculate_det),
    UNIT_TEST_ITEM(should_fail_to_calc_det_if_matrix_not_square),
    UNIT_TEST_ITEM(should_calculate_rank),
    UNIT_TEST_ITEM(should_lu_decomposition),
    UNIT_TEST_ITEM(should_fail_to_lu_decomposition_if_matrix_not_square),
    UNIT_TEST_ITEM(should_cmp),
    UNIT_TEST_ITEM(should_fail_to_cmp_if_matrix_not_match),
    UNIT_TEST_ITEM(test_matrix_view))
} // namespace my::test::test_matrix