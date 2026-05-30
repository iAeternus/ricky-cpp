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

// Vec regression tests for iterator constructor, resize, try_expand fixes
void should_construct_from_iterator_range();
void should_push_after_iterator_construction();
void should_construct_from_empty_iterator_range();
void should_push_large_after_iterator_construction();
void should_construct_from_iterator_range_with_nontrivial_type();
void should_resize_up_with_non_trivial_types();
void should_resize_down_preserves_content();
void should_handle_multiple_resizes();

} // namespace my::test::test_vec

#endif // TEST_VEC_HPP