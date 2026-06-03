#ifndef TEST_ADAM_HPP
#define TEST_ADAM_HPP

namespace my::test::test_adam {

void should_construct();
void should_step();
void should_step_with_weight_decay();
void should_zero_grad();
void should_skip_when_grad_empty();

} // namespace my::test::test_adam

#endif // TEST_ADAM_HPP
