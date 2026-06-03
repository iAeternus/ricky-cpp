#include "test_dataset.hpp"
#include "dataset.hpp"
#include "ricky_test.hpp"

namespace my::test::test_dataset {

using Tensor = nn::Tensor<i32>;
using FTensor = nn::Tensor<f32>;
using Shape = typename Tensor::Shape;
using Dataset = nn::data::TensorDataset<i32>;

void should_construct_empty() {
    Dataset ds;

    Assertions::assert_equals(static_cast<usize>(0), ds.size());
}

void should_construct_with_samples() {
    util::Vec<Pair<Tensor, Tensor>> samples;
    samples.push(Pair(Tensor::scalar(1), Tensor::scalar(10)));
    samples.push(Pair(Tensor::scalar(2), Tensor::scalar(20)));

    Dataset ds(samples);

    Assertions::assert_equals(static_cast<usize>(2), ds.size());
}

void should_construct_with_tensors() {
    Tensor x(Shape{3, 2});
    x(0, 0) = 1; x(0, 1) = 2;
    x(1, 0) = 3; x(1, 1) = 4;
    x(2, 0) = 5; x(2, 1) = 6;

    Tensor y = Tensor::scalar(0);
    y = Tensor(Shape{3});
    y(0) = 10; y(1) = 20; y(2) = 30;

    Dataset ds(x, y);

    Assertions::assert_equals(static_cast<usize>(3), ds.size());
}

void should_get_sample() {
    util::Vec<Pair<Tensor, Tensor>> samples;
    samples.push(Pair(Tensor::scalar(1), Tensor::scalar(10)));
    samples.push(Pair(Tensor::scalar(2), Tensor::scalar(20)));

    Dataset ds(samples);
    auto sample = ds.get(1);

    Assertions::assert_equals(2, sample.first().data()[0]);
    Assertions::assert_equals(20, sample.second().data()[0]);
}

void should_get_input_and_target() {
    util::Vec<Pair<Tensor, Tensor>> samples;
    samples.push(Pair(Tensor::scalar(5), Tensor::scalar(50)));

    Dataset ds(samples);

    Assertions::assert_equals(5, ds.get_input(0).data()[0]);
    Assertions::assert_equals(50, ds.get_target(0).data()[0]);
}

void should_push_sample() {
    Dataset ds;
    ds.push(Tensor::scalar(7), Tensor::scalar(70));
    ds.push(Tensor::scalar(8), Tensor::scalar(80));

    Assertions::assert_equals(static_cast<usize>(2), ds.size());
    Assertions::assert_equals(7, ds.get(0).first().data()[0]);
    Assertions::assert_equals(80, ds.get(1).second().data()[0]);
}

void should_subset_by_indices() {
    util::Vec<Pair<Tensor, Tensor>> samples;
    samples.push(Pair(Tensor::scalar(1), Tensor::scalar(10)));
    samples.push(Pair(Tensor::scalar(2), Tensor::scalar(20)));
    samples.push(Pair(Tensor::scalar(3), Tensor::scalar(30)));

    Dataset ds(samples);
    util::Vec<usize> indices;
    indices.push(static_cast<usize>(0));
    indices.push(static_cast<usize>(2));

    Dataset subset(ds, indices);

    Assertions::assert_equals(static_cast<usize>(2), subset.size());
    Assertions::assert_equals(1, subset.get(0).first().data()[0]);
    Assertions::assert_equals(30, subset.get(1).second().data()[0]);
}

GROUP_NAME("test_dataset")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(should_construct_empty),
    UNIT_TEST_ITEM(should_construct_with_samples),
    UNIT_TEST_ITEM(should_construct_with_tensors),
    UNIT_TEST_ITEM(should_get_sample),
    UNIT_TEST_ITEM(should_get_input_and_target),
    UNIT_TEST_ITEM(should_push_sample),
    UNIT_TEST_ITEM(should_subset_by_indices))

} // namespace my::test::test_dataset
