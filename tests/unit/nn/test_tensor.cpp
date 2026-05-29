#include "test_tensor.hpp"
#include "tensor.hpp"
#include "ricky_test.hpp"

namespace my::test::test_tensor {

using Tensor = nn::Tensor<i32>;

void should_construct() {
    // Given
    Tensor::Shape shape{2, 3, 4};

    // When
    Tensor tensor(shape);

    // Then
    Assertions::assert_equals(static_cast<usize>(3), tensor.ndim());
    Assertions::assert_equals(static_cast<usize>(24), tensor.numel());

    Assertions::assert_equals(static_cast<usize>(2), tensor.shape()[0]);
    Assertions::assert_equals(static_cast<usize>(3), tensor.shape()[1]);
    Assertions::assert_equals(static_cast<usize>(4), tensor.shape()[2]);

    Assertions::assert_equals(static_cast<usize>(12), tensor.stride()[0]);
    Assertions::assert_equals(static_cast<usize>(4), tensor.stride()[1]);
    Assertions::assert_equals(static_cast<usize>(1), tensor.stride()[2]);

    Assertions::assert_true(tensor.is_contiguous());
}

void should_construct_scalar() {
    Tensor tensor(Tensor::Shape{});

    Assertions::assert_equals(static_cast<usize>(0), tensor.ndim());
    Assertions::assert_equals(static_cast<usize>(1), tensor.numel());
    Assertions::assert_true(tensor.is_contiguous());
}

void should_construct_with_value() {
    Tensor tensor(Tensor::Shape{2, 2}, 7);

    Assertions::assert_equals(7, tensor(0, 0));
    Assertions::assert_equals(7, tensor(0, 1));
    Assertions::assert_equals(7, tensor(1, 0));
    Assertions::assert_equals(7, tensor(1, 1));
}

void should_create_zeros() {
    auto tensor = Tensor::zeros({2, 3});

    for (usize i = 0; i < 2; ++i) {
        for (usize j = 0; j < 3; ++j) {
            Assertions::assert_equals(0, tensor(i, j));
        }
    }
}

void should_create_ones() {
    auto tensor = Tensor::ones({2, 3});

    for (usize i = 0; i < 2; ++i) {
        for (usize j = 0; j < 3; ++j) {
            Assertions::assert_equals(1, tensor(i, j));
        }
    }
}

void should_create_arange() {
    auto tensor = Tensor::arange(0, 5);

    Assertions::assert_equals(static_cast<usize>(5), tensor.numel());
    Assertions::assert_equals(0, tensor(0));
    Assertions::assert_equals(1, tensor(1));
    Assertions::assert_equals(2, tensor(2));
    Assertions::assert_equals(3, tensor(3));
    Assertions::assert_equals(4, tensor(4));
}

void should_throw_when_arange_step_is_zero() {
    Assertions::assert_throws<Exception>([] {
        Tensor::arange(0, 10, 0);
    });
}

void should_access_element() {
    // Given
    Tensor tensor({2, 3});

    // When
    tensor(0, 0) = 1;
    tensor(0, 1) = 2;
    tensor(1, 2) = 7;

    // Then
    Assertions::assert_equals(1, tensor(0, 0));
    Assertions::assert_equals(2, tensor(0, 1));
    Assertions::assert_equals(7, tensor(1, 2));
}

void should_throw_when_index_out_of_range() {
    // Given
    Tensor tensor({2, 3});

    // When & Then
    Assertions::assert_throws<Exception>([&] {
        tensor(2, 0);
    });

    Assertions::assert_throws<Exception>([&] {
        tensor(0, 3);
    });
}

void should_view_tensor() {
    // Given
    auto tensor = Tensor::arange(0, 6);

    // When
    auto view = tensor.view({2, 3});

    // Then
    Assertions::assert_equals(static_cast<usize>(2), view.shape()[0]);
    Assertions::assert_equals(static_cast<usize>(3), view.shape()[1]);

    Assertions::assert_equals(0, view(0, 0));
    Assertions::assert_equals(1, view(0, 1));
    Assertions::assert_equals(5, view(1, 2));

    Assertions::assert_true(view.is_contiguous());
}

void should_throw_when_view_non_contiguous() {
    // Given
    auto tensor = Tensor::arange(0, 6).view({2, 3});
    auto transposed = tensor.transpose(0, 1);

    // When & Then
    Assertions::assert_false(transposed.is_contiguous());

    Assertions::assert_throws<Exception>([&] {
        auto _ = transposed.view({6});
    });
}

void should_reshape_non_contiguous() {
    // Given
    auto tensor = Tensor::arange(0, 6).view({2, 3});
    auto transposed = tensor.transpose(0, 1);

    // When
    auto reshaped = transposed.reshape({6});

    // Then
    Assertions::assert_true(reshaped.is_contiguous());

    Assertions::assert_equals(0, reshaped(0));
    Assertions::assert_equals(3, reshaped(1));
    Assertions::assert_equals(1, reshaped(2));
    Assertions::assert_equals(4, reshaped(3));
    Assertions::assert_equals(2, reshaped(4));
    Assertions::assert_equals(5, reshaped(5));
}

void should_transpose() {
    // Given
    auto tensor = Tensor::arange(0, 6).view({2, 3});

    // When
    auto transposed = tensor.transpose(0, 1);

    // Then
    Assertions::assert_equals(static_cast<usize>(3), transposed.shape()[0]);
    Assertions::assert_equals(static_cast<usize>(2), transposed.shape()[1]);

    Assertions::assert_equals(0, transposed(0, 0));
    Assertions::assert_equals(3, transposed(0, 1));

    Assertions::assert_equals(1, transposed(1, 0));
    Assertions::assert_equals(4, transposed(1, 1));

    Assertions::assert_false(transposed.is_contiguous());
}

void should_permute() {
    // Given
    auto tensor = Tensor::arange(0, 24).view({2, 3, 4});

    // When
    auto permuted = tensor.permute({2, 0, 1});

    // Then
    Assertions::assert_equals(static_cast<usize>(4), permuted.shape()[0]);
    Assertions::assert_equals(static_cast<usize>(2), permuted.shape()[1]);
    Assertions::assert_equals(static_cast<usize>(3), permuted.shape()[2]);

    Assertions::assert_equals(0, permuted(0, 0, 0));
    Assertions::assert_equals(1, permuted(1, 0, 0));
    Assertions::assert_equals(12, permuted(0, 1, 0));

    Assertions::assert_false(permuted.is_contiguous());
}

void should_slice() {
    // Given
    auto tensor = Tensor::arange(0, 12).view({3, 4});

    // When
    auto slice = tensor.slice(0, 1, 3);

    // Then
    Assertions::assert_equals(static_cast<usize>(2), slice.shape()[0]);
    Assertions::assert_equals(static_cast<usize>(4), slice.shape()[1]);

    Assertions::assert_equals(4, slice(0, 0));
    Assertions::assert_equals(5, slice(0, 1));
    Assertions::assert_equals(11, slice(1, 3));
}

void should_flatten() {
    // Given
    auto tensor = Tensor::arange(0, 12).view({3, 4});

    // When
    auto flat = tensor.flatten();

    // Then
    Assertions::assert_equals(static_cast<usize>(1), flat.ndim());
    Assertions::assert_equals(static_cast<usize>(12), flat.numel());

    for (usize i = 0; i < 12; ++i) {
        Assertions::assert_equals(static_cast<i32>(i), flat(i));
    }
}

void should_unsqueeze() {
    // Given
    auto tensor = Tensor::arange(0, 6).view({2, 3});

    // When
    auto unsqueezed = tensor.unsqueeze(1);

    // Then
    Assertions::assert_equals(static_cast<usize>(3), unsqueezed.ndim());

    Assertions::assert_equals(static_cast<usize>(2), unsqueezed.shape()[0]);
    Assertions::assert_equals(static_cast<usize>(1), unsqueezed.shape()[1]);
    Assertions::assert_equals(static_cast<usize>(3), unsqueezed.shape()[2]);

    Assertions::assert_equals(5, unsqueezed(1, 0, 2));
}

void should_squeeze() {
    // Given
    auto tensor = Tensor::arange(0, 6).view({2, 1, 3});

    // When
    auto squeezed = tensor.squeeze(1);

    // Then
    Assertions::assert_equals(static_cast<usize>(2), squeezed.ndim());

    Assertions::assert_equals(static_cast<usize>(2), squeezed.shape()[0]);
    Assertions::assert_equals(static_cast<usize>(3), squeezed.shape()[1]);

    Assertions::assert_equals(5, squeezed(1, 2));
}

void should_squeeze_all() {
    // Given
    auto tensor = Tensor::arange(0, 6).view({1, 2, 1, 3, 1});

    // When
    auto squeezed = tensor.squeeze();

    // Then
    Assertions::assert_equals(static_cast<usize>(2), squeezed.ndim());

    Assertions::assert_equals(static_cast<usize>(2), squeezed.shape()[0]);
    Assertions::assert_equals(static_cast<usize>(3), squeezed.shape()[1]);

    Assertions::assert_equals(5, squeezed(1, 2));
}

void should_contiguous() {
    // Given
    auto tensor = Tensor::arange(0, 6).view({2, 3});
    auto transposed = tensor.transpose(0, 1);

    // When
    auto contiguous = transposed.contiguous();

    // Then
    Assertions::assert_true(contiguous.is_contiguous());

    Assertions::assert_equals(0, contiguous(0, 0));
    Assertions::assert_equals(3, contiguous(0, 1));

    Assertions::assert_equals(1, contiguous(1, 0));
    Assertions::assert_equals(4, contiguous(1, 1));
}

void should_detect_contiguous() {
    // Given
    auto tensor = Tensor::arange(0, 12).view({3, 4});

    // When
    auto transposed = tensor.transpose(0, 1);

    // Then
    Assertions::assert_true(tensor.is_contiguous());
    Assertions::assert_false(transposed.is_contiguous());
}

void should_share_storage_in_view() {
    // Given
    auto tensor = Tensor::arange(0, 6).view({2, 3});

    // When
    auto view = tensor.view({3, 2});
    view(1, 0) = 999;

    // Then
    Assertions::assert_equals(999, tensor(0, 2));
}

void should_copy_storage_in_contiguous() {
    // Given
    auto tensor = Tensor::arange(0, 6).view({2, 3});
    auto transposed = tensor.transpose(0, 1);

    // When
    auto contiguous = transposed.contiguous();
    contiguous(0, 0) = 999;

    // Then
    Assertions::assert_equals(0, tensor(0, 0));
    Assertions::assert_equals(999, contiguous(0, 0));
}

GROUP_NAME("test_tensor")

REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(should_construct),
    UNIT_TEST_ITEM(should_construct_scalar),
    UNIT_TEST_ITEM(should_construct_with_value),
    UNIT_TEST_ITEM(should_create_zeros),
    UNIT_TEST_ITEM(should_create_ones),
    UNIT_TEST_ITEM(should_create_arange),
    UNIT_TEST_ITEM(should_throw_when_arange_step_is_zero),
    UNIT_TEST_ITEM(should_access_element),
    UNIT_TEST_ITEM(should_throw_when_index_out_of_range),
    UNIT_TEST_ITEM(should_view_tensor),
    UNIT_TEST_ITEM(should_throw_when_view_non_contiguous),
    UNIT_TEST_ITEM(should_reshape_non_contiguous),
    UNIT_TEST_ITEM(should_transpose),
    UNIT_TEST_ITEM(should_permute),
    UNIT_TEST_ITEM(should_slice),
    UNIT_TEST_ITEM(should_flatten),
    UNIT_TEST_ITEM(should_unsqueeze),
    UNIT_TEST_ITEM(should_squeeze),
    UNIT_TEST_ITEM(should_squeeze_all),
    UNIT_TEST_ITEM(should_contiguous),
    UNIT_TEST_ITEM(should_detect_contiguous),
    UNIT_TEST_ITEM(should_share_storage_in_view),
    UNIT_TEST_ITEM(should_copy_storage_in_contiguous))

} // namespace my::test::test_tensor