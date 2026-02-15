#ifndef TEST_VEC_HPP
#define TEST_VEC_HPP

namespace my::test::test_vec {

void it_works();
void should_append();
void should_insert();
void should_pop();
void should_pop2();
void should_clear();
void should_swap();
void should_to_array();
void should_slice();
void should_extend();
void should_at();
void should_find();
void should_sort();
void test_opt();
void should_fail_to_opt_if_index_out_of_bounds();
void should_fail_to_opt_if_type_mismatch();

void speed_of_util_vec_append_string();
void speed_of_std_vector_push_back_string();
void speed_of_util_vec_append_i32();
void speed_of_std_vector_push_back_i32();
void test_vec_speed();

} // namespace my::test::test_vec

#endif // TEST_VEC_HPP