#include "test_dataloader.hpp"
#include "dataloader.hpp"
#include "ricky_test.hpp"

namespace my::test::test_dataloader {

using Tensor = nn::Tensor<i32>;
using Shape = typename Tensor::Shape;
using Dataset = nn::data::TensorDataset<i32>;
using DataLoader = nn::data::DataLoader<i32>;

Tensor make_x(i32 val) {
    Tensor t(Shape{1});
    t.data()[0] = val;
    return t;
}

Tensor make_y(i32 val) {
    return Tensor::scalar(val);
}

void should_construct_and_iterate() {
    Dataset ds;
    ds.push(make_x(1), make_y(10));
    ds.push(make_x(2), make_y(20));
    ds.push(make_x(3), make_y(30));
    ds.push(make_x(4), make_y(40));

    DataLoader loader(ds, 2, false);

    Assertions::assert_true(loader.has_next());
    Assertions::assert_equals(static_cast<usize>(4), loader.size());
    Assertions::assert_equals(static_cast<usize>(2), loader.batch_size());
}

void should_return_correct_batch_size() {
    Dataset ds;
    ds.push(make_x(1), make_y(10));
    ds.push(make_x(2), make_y(20));
    ds.push(make_x(3), make_y(30));
    ds.push(make_x(4), make_y(40));

    DataLoader loader(ds, 2, false);

    auto batch = loader.next_batch();
    Assertions::assert_equals(static_cast<usize>(2), batch.first().shape()[0]);
    Assertions::assert_equals(static_cast<usize>(1), batch.first().shape()[1]);

    batch = loader.next_batch();
    Assertions::assert_equals(static_cast<usize>(2), batch.first().shape()[0]);
}

void should_reset() {
    Dataset ds;
    ds.push(make_x(1), make_y(10));
    ds.push(make_x(2), make_y(20));

    DataLoader loader(ds, 2, false);

    loader.next_batch();
    Assertions::assert_false(loader.has_next());

    loader.reset();
    Assertions::assert_true(loader.has_next());
}

void should_handle_remaining_batch() {
    Dataset ds;
    ds.push(make_x(1), make_y(10));
    ds.push(make_x(2), make_y(20));
    ds.push(make_x(3), make_y(30));

    DataLoader loader(ds, 2, false);

    auto batch1 = loader.next_batch();
    Assertions::assert_equals(static_cast<usize>(2), batch1.first().shape()[0]);

    auto batch2 = loader.next_batch();
    Assertions::assert_equals(static_cast<usize>(1), batch2.first().shape()[0]);

    Assertions::assert_false(loader.has_next());
}

void should_shuffle_with_reset() {
    Dataset ds;
    ds.push(make_x(1), make_y(10));
    ds.push(make_x(2), make_y(20));
    ds.push(make_x(3), make_y(30));
    ds.push(make_x(4), make_y(40));

    DataLoader loader(ds, 2, true);

    loader.next_batch();
    loader.next_batch();
    Assertions::assert_false(loader.has_next());

    loader.reset();
    Assertions::assert_true(loader.has_next());
}

GROUP_NAME("test_dataloader")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(should_construct_and_iterate),
    UNIT_TEST_ITEM(should_return_correct_batch_size),
    UNIT_TEST_ITEM(should_reset),
    UNIT_TEST_ITEM(should_handle_remaining_batch),
    UNIT_TEST_ITEM(should_shuffle_with_reset))

} // namespace my::test::test_dataloader
