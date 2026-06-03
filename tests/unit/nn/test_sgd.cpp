#include "test_sgd.hpp"
#include "sgd.hpp"
#include "ricky_test.hpp"

namespace my::test::test_sgd {

using FTensor = nn::Tensor<f32>;
using SGD = nn::optim::SGD<f32>;

void should_construct() {
    FTensor w = FTensor::ones(FTensor::Shape{2, 2});
    w.set_requires_grad(true);

    util::Vec<FTensor*> params;
    params.push(&w);

    SGD optim(params, 0.01f);

    Assertions::assert_true(true);
}

void should_step_without_momentum() {
    FTensor w = FTensor::ones(FTensor::Shape{2, 2});
    w.set_requires_grad(true);

    w.grad() = FTensor::ones(FTensor::Shape{2, 2});

    for (usize i = 0; i < 2; ++i) {
        for (usize j = 0; j < 2; ++j) {
            w.grad()(i, j) = 2.0f;
        }
    }

    util::Vec<FTensor*> params;
    params.push(&w);

    SGD optim(params, 0.1f);
    optim.step();

    // w = w - lr * grad = 1.0 - 0.1 * 2.0 = 0.8
    for (usize i = 0; i < 2; ++i) {
        for (usize j = 0; j < 2; ++j) {
            Assertions::assert_equals(0.8f, w(i, j));
        }
    }
}

void should_step_with_momentum() {
    FTensor w = FTensor::scalar(1.0f);
    w.set_requires_grad(true);
    w.grad() = FTensor::scalar(2.0f);

    util::Vec<FTensor*> params;
    params.push(&w);

    SGD optim(params, 0.1f, 0.9f);
    optim.step();

    // v = momentum * 0 + grad = 2.0
    // w = w - lr * v = 1.0 - 0.1 * 2.0 = 0.8
    Assertions::assert_equals(0.8f, w.data()[0]);

    w.grad() = FTensor::scalar(1.0f);
    optim.step();

    // v = 0.9 * 2.0 + 1.0 = 2.8
    // w = 0.8 - 0.1 * 2.8 = 0.52
    Assertions::assert_equals(0.52f, w.data()[0]);
}

void should_step_with_weight_decay() {
    FTensor w = FTensor::scalar(1.0f);
    w.set_requires_grad(true);
    w.grad() = FTensor::scalar(2.0f);

    util::Vec<FTensor*> params;
    params.push(&w);

    SGD optim(params, 0.1f, 0.0f, 0.5f);
    optim.step();

    // grad = grad + weight_decay * w = 2.0 + 0.5 * 1.0 = 2.5
    // w = w - lr * grad = 1.0 - 0.1 * 2.5 = 0.75
    Assertions::assert_equals(0.75f, w.data()[0]);
}

void should_zero_grad() {
    FTensor w = FTensor::scalar(1.0f);
    w.set_requires_grad(true);

    w.grad() = FTensor::scalar(5.0f);
    Assertions::assert_false(w.grad().is_empty());

    util::Vec<FTensor*> params;
    params.push(&w);

    SGD optim(params, 0.1f);
    optim.zero_grad();

    Assertions::assert_true(w.grad().is_empty());
}

void should_skip_when_grad_empty() {
    FTensor w = FTensor::scalar(1.0f);
    w.set_requires_grad(true);

    util::Vec<FTensor*> params;
    params.push(&w);

    SGD optim(params, 0.1f);
    optim.step();

    Assertions::assert_equals(1.0f, w.data()[0]);
}

GROUP_NAME("test_sgd")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(should_construct),
    UNIT_TEST_ITEM(should_step_without_momentum),
    UNIT_TEST_ITEM(should_step_with_momentum),
    UNIT_TEST_ITEM(should_step_with_weight_decay),
    UNIT_TEST_ITEM(should_zero_grad),
    UNIT_TEST_ITEM(should_skip_when_grad_empty))

} // namespace my::test::test_sgd
