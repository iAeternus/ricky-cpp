#ifndef TEST_TENSOR_HPP
#define TEST_TENSOR_HPP

namespace my::test::test_tensor {

void should_construct();
void should_construct_scalar();
void should_construct_with_value();

void should_create_zeros();
void should_create_ones();
void should_create_arange();
void should_throw_when_arange_step_is_zero();
void should_arange_with_step_and_negative();
void should_arange_empty_case_positive();
void should_arange_empty_case_negative();

void should_access_element();
void should_throw_when_index_out_of_range();

void should_view_tensor();
void should_throw_when_view_non_contiguous();

void should_reshape_non_contiguous();

void should_transpose();

void should_permute();
void should_throw_when_permute_dim_mismatch();
void should_throw_when_permute_invalid_dim();
void should_throw_when_permute_duplicate_dim();

void should_slice();
void should_throw_when_slice_dim_out_of_range();
void should_throw_when_slice_invalid_range();

void should_flatten();

void should_unsqueeze();
void should_throw_when_unsqueeze_dim_invalid();

void should_squeeze();
void should_throw_when_squeeze_not_one();
void should_throw_when_squeeze_dim_out_of_range();

void should_squeeze_all();

void should_contiguous();
void should_detect_contiguous();
void should_contiguous_false_for_transposed();

void should_share_storage_in_view();
void should_copy_storage_in_contiguous();

void should_throw_when_broadcast_shape_mismatch();
void should_broadcast_add_scalar();

void should_throw_on_div_zero();

// void should_matmul_basic_2d();
// void should_throw_matmul_shape_mismatch();
// void should_matmul_batch();

void should_throw_mean_empty();
void should_throw_max_empty();

void should_elementwise_shape_mismatch();

} // namespace my::test::test_tensor

#endif // TEST_TENSOR_HPP