#ifndef TEST_SGD_HPP
#define TEST_SGD_HPP

namespace my::test::test_sgd {

void should_construct();
void should_step_without_momentum();
void should_step_with_momentum();
void should_step_with_weight_decay();
void should_zero_grad();
void should_skip_when_grad_empty();

} // namespace my::test::test_sgd

#endif // TEST_SGD_HPP
