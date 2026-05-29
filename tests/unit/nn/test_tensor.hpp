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
void should_access_element();
void should_throw_when_index_out_of_range();
void should_view_tensor();
void should_throw_when_view_non_contiguous();
void should_reshape_non_contiguous();
void should_transpose();
void should_permute();
void should_slice();
void should_flatten();
void should_unsqueeze();
void should_squeeze();
void should_squeeze_all();
void should_contiguous();
void should_detect_contiguous();
void should_share_storage_in_view();
void should_copy_storage_in_contiguous();

} // namespace my::test::test_tensor

#endif // TEST_TENSOR_HPP