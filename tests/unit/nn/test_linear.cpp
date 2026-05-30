#include "test_linear.hpp"
#include "linear.hpp"
#include "ricky_test.hpp"

namespace my::test::test_linear {

using Tensor = nn::Tensor<f32>;
using Shape = typename Tensor::Shape;

void should_construct_linear() {
    nn::Linear<f32> linear(3, 2);

    auto params = linear.parameters();
    // weight + bias
    Assertions::assert_equals(static_cast<usize>(2), params.len());

    // weight shape: (2, 3)
    Assertions::assert_equals(static_cast<usize>(2), linear.weight_.shape()[0]);
    Assertions::assert_equals(static_cast<usize>(3), linear.weight_.shape()[1]);

    // bias shape: (2,)
    Assertions::assert_equals(static_cast<usize>(2), linear.bias_.shape()[0]);
}

void should_linear_forward() {
    nn::Linear<f32> linear(4, 2);

    Tensor input = Tensor::ones(Shape{3, 4});
    Tensor output = linear.forward(input);

    // output shape: (3, 2)
    Assertions::assert_equals(static_cast<usize>(2), output.ndim());
    Assertions::assert_equals(static_cast<usize>(3), output.shape()[0]);
    Assertions::assert_equals(static_cast<usize>(2), output.shape()[1]);
}

void should_linear_parameters() {
    nn::Linear<f32> linear_no_bias(3, 2, false);

    auto params = linear_no_bias.parameters();
    Assertions::assert_equals(static_cast<usize>(1), params.len());
    Assertions::assert_true(linear_no_bias.bias_.is_empty());
}

GROUP_NAME("test_linear")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(should_construct_linear),
    UNIT_TEST_ITEM(should_linear_forward),
    UNIT_TEST_ITEM(should_linear_parameters))

} // namespace my::test::test_linear
