#include "test_adam.hpp"
#include "adam.hpp"
#include "ricky_test.hpp"

namespace my::test::test_adam {

using FTensor = nn::Tensor<f32>;
using Adam = nn::optim::Adam<f32>;

void should_construct() {
    FTensor w = FTensor::ones(FTensor::Shape{2, 2});
    w.set_requires_grad(true);

    util::Vec<FTensor*> params;
    params.push(&w);

    Adam optim(params, 0.001f);

    Assertions::assert_true(true);
}

void should_step() {
    FTensor w = FTensor::scalar(1.0f);
    w.set_requires_grad(true);
    w.grad() = FTensor::scalar(2.0f);

    util::Vec<FTensor*> params;
    params.push(&w);

    Adam optim(params, 0.1f);
    optim.step();

    // After one step with lr=0.1, w should be different from 1.0
    Assertions::assert_not_equals(1.0f, w.data()[0]);

    // The update should be negative (gradient descent: w = w - update)
    Assertions::assert_true(w.data()[0] < 1.0f);
}

void should_step_with_weight_decay() {
    FTensor w = FTensor::scalar(1.0f);
    w.set_requires_grad(true);
    w.grad() = FTensor::scalar(2.0f);

    util::Vec<FTensor*> params;
    params.push(&w);

    Adam optim(params, 0.1f, 0.9f, 0.999f, 1e-8f, 0.5f);
    optim.step();

    Assertions::assert_true(w.data()[0] < 1.0f);
}

void should_zero_grad() {
    FTensor w = FTensor::scalar(1.0f);
    w.set_requires_grad(true);
    w.grad() = FTensor::scalar(5.0f);

    Assertions::assert_false(w.grad().is_empty());

    util::Vec<FTensor*> params;
    params.push(&w);

    Adam optim(params, 0.001f);
    optim.zero_grad();

    Assertions::assert_true(w.grad().is_empty());
}

void should_skip_when_grad_empty() {
    FTensor w = FTensor::scalar(1.0f);
    w.set_requires_grad(true);

    util::Vec<FTensor*> params;
    params.push(&w);

    Adam optim(params, 0.1f);
    optim.step();

    Assertions::assert_equals(1.0f, w.data()[0]);
}

GROUP_NAME("test_adam")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(should_construct),
    UNIT_TEST_ITEM(should_step),
    UNIT_TEST_ITEM(should_step_with_weight_decay),
    UNIT_TEST_ITEM(should_zero_grad),
    UNIT_TEST_ITEM(should_skip_when_grad_empty))

} // namespace my::test::test_adam
