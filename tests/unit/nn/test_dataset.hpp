#ifndef TEST_DATASET_HPP
#define TEST_DATASET_HPP

namespace my::test::test_dataset {

void should_construct_empty();
void should_construct_with_samples();
void should_construct_with_tensors();
void should_get_sample();
void should_get_input_and_target();
void should_push_sample();
void should_subset_by_indices();

} // namespace my::test::test_dataset

#endif // TEST_DATASET_HPP
