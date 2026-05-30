#include "test_activations.hpp"
#include "activations.hpp"
#include "ricky_test.hpp"

namespace my::test::test_activations {

using Tensor = nn::Tensor<f32>;
using Shape = typename Tensor::Shape;

void should_relu_forward() {
    nn::ReLU<f32> relu;

    // [-2, -1, 0, 1, 2]
    Tensor input = Tensor::arange(-2, 3).view(Shape{5});
    Tensor output = relu.forward(input);

    Assertions::assert_equals(static_cast<f32>(0), output(0));
    Assertions::assert_equals(static_cast<f32>(0), output(1));
    Assertions::assert_equals(static_cast<f32>(0), output(2));
    Assertions::assert_equals(static_cast<f32>(1), output(3));
    Assertions::assert_equals(static_cast<f32>(2), output(4));
}

void should_sigmoid_forward() {
    nn::Sigmoid<f32> sigmoid;

    Tensor input = Tensor::scalar(0.0f);
    Tensor output = sigmoid.forward(input);

    // sigmoid(0) = 0.5
    Assertions::assert_equals(static_cast<f32>(0.5f), output.data()[0]);
}

void should_softmax_forward() {
    nn::Softmax<f32> softmax;

    // [1, 2, 3]
    Tensor input = Tensor::arange(1, 4).view(Shape{3});
    Tensor output = softmax.forward(input);

    // sum of softmax outputs = 1
    f32 sum = 0;
    for (usize i = 0; i < output.numel(); ++i) {
        sum += output.data()[i];
    }
    Assertions::assert_equals(static_cast<f32>(1.0f), sum);

    // softmax values are positive
    for (usize i = 0; i < output.numel(); ++i) {
        Assertions::assert_true(output.data()[i] > 0);
    }
}

GROUP_NAME("test_activations")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(should_relu_forward),
    UNIT_TEST_ITEM(should_sigmoid_forward),
    UNIT_TEST_ITEM(should_softmax_forward))

} // namespace my::test::test_activations
