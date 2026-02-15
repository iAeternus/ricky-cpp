#ifndef TEST_MATRIX_HPP
#define TEST_MATRIX_HPP

#include "my_types.hpp"

namespace my::test::test_matrix {
void should_construct();
void should_at();
void should_fail_to_get_if_index_out_of_bounds();
void should_get_sub_mat();
void should_fail_to_get_sub_mat_if_index_invalid();
void should_fill();
void should_add();
void should_fail_to_add_if_matrix_not_match();
void should_subtract();
void should_fail_to_subtract_if_matrix_not_match();
void should_multiply();
void should_fail_to_multiply_if_matrix_not_match();
void should_dot();
void should_fail_to_dot_if_matrix_not_match();
void should_transpose();
void should_calculate_inverse();
void should_fail_to_calc_inv_if_matrix_not_square();
void should_calculate_det();
void should_fail_to_calc_det_if_matrix_not_square();
void should_calculate_rank();
void should_lu_decomposition();
void should_fail_to_lu_decomposition_if_matrix_not_square();
void should_cmp();
void should_fail_to_cmp_if_matrix_not_match();
void test_matrix_view();
void test_matrix();
} // namespace my::test::test_matrix

#endif // TEST_MATRIX_HPP