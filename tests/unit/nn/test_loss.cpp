#include "test_loss.hpp"
#include "loss.hpp"
#include "ricky_test.hpp"

namespace my::test::test_loss {

using Tensor = nn::Tensor<f32>;
using Shape = typename Tensor::Shape;

void should_mse_loss_forward() {
    nn::MSELoss<f32> mse;

    Tensor input = Tensor::arange(0, 4).view(Shape{2, 2});  // [[0,1],[2,3]]
    Tensor target = Tensor::ones(Shape{2, 2});               // [[1,1],[1,1]]

    Tensor loss = mse.forward(input, target);

    // (0-1)^2 + (1-1)^2 + (2-1)^2 + (3-1)^2 = 1+0+1+4 = 6, mean = 6/4 = 1.5
    Assertions::assert_equals(static_cast<f32>(1.5f), loss.data()[0]);
}

void should_cross_entropy_loss_forward() {
    nn::CrossEntropyLoss<f32> cel;

    // batch=2, num_classes=3
    Tensor input = Tensor::arange(0, 6).view(Shape{2, 3});
    Tensor target = Tensor(Shape{2}, static_cast<f32>(0));  // [0, 0]

    // Should not throw and produce a scalar loss
    Tensor loss = cel.forward(input, target);

    Assertions::assert_equals(static_cast<usize>(0), loss.ndim());
    Assertions::assert_true(loss.data()[0] > 0);
}

GROUP_NAME("test_loss")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(should_mse_loss_forward),
    UNIT_TEST_ITEM(should_cross_entropy_loss_forward))

} // namespace my::test::test_loss
