#include "test_autograd.hpp"
#include "autograd.hpp"
#include "ricky_test.hpp"

namespace my::test::test_autograd {

using Tensor = nn::Tensor<f32>;
using namespace nn;

void should_track_gradients() {
    Tensor a = Tensor::scalar(2.0f);
    a.set_requires_grad(true);

    Assertions::assert_true(a.requires_grad());
    Assertions::assert_true(a.grad().is_empty());

    // detach removes tracking
    auto b = a.detach();
    Assertions::assert_false(b.requires_grad());
}

void should_backward_simple_add() {
    Tensor a = Tensor::scalar(2.0f).set_requires_grad(true);
    Tensor b = Tensor::scalar(3.0f).set_requires_grad(true);

    Tensor c = autograd_add(a, b);  // c = 5

    Assertions::assert_false(c.grad_fn() == nullptr);

    c.backward();

    Assertions::assert_equals(static_cast<f32>(1.0f), a.grad().data()[0]);
    Assertions::assert_equals(static_cast<f32>(1.0f), b.grad().data()[0]);
}

void should_backward_chain() {
    Tensor a = Tensor::scalar(2.0f).set_requires_grad(true);
    Tensor b = Tensor::scalar(3.0f).set_requires_grad(true);

    // c = a + b = 5
    Tensor c = autograd_add(a, b);

    // d = c * a = 5 * 2 = 10
    Tensor d = autograd_mul(c, a);

    d.backward();

    // dd/da = c + a * dc/da = c + a * 1 = 5 + 2 = 7
    // dd/db = a * dc/db = a * 1 = 2
    Assertions::assert_equals(static_cast<f32>(7.0f), a.grad().data()[0]);
    Assertions::assert_equals(static_cast<f32>(2.0f), b.grad().data()[0]);
}

void should_detach() {
    Tensor a = Tensor::scalar(2.0f).set_requires_grad(true);
    Tensor b = Tensor::scalar(3.0f).set_requires_grad(true);

    Tensor c = autograd_add(a, b);  // c = 5
    Tensor d = c.detach();           // detached, no grad tracking

    Tensor e = autograd_mul(d, a);

    Assertions::assert_true(c.grad_fn() != nullptr);
    Assertions::assert_true(d.grad_fn() == nullptr);
    Assertions::assert_false(d.requires_grad());
}

void should_zero_grad() {
    Tensor a = Tensor::scalar(2.0f).set_requires_grad(true);
    Tensor b = Tensor::scalar(3.0f).set_requires_grad(true);

    Tensor c = autograd_add(a, b);
    c.backward();

    Assertions::assert_false(a.grad().is_empty());

    a.zero_grad();
    Assertions::assert_true(a.grad().is_empty());
}

GROUP_NAME("test_autograd")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(should_track_gradients),
    UNIT_TEST_ITEM(should_backward_simple_add),
    UNIT_TEST_ITEM(should_backward_chain),
    UNIT_TEST_ITEM(should_detach),
    UNIT_TEST_ITEM(should_zero_grad))

} // namespace my::test::test_autograd
