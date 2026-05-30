#ifndef TEST_AUTOGRAD_HPP
#define TEST_AUTOGRAD_HPP

namespace my::test::test_autograd {

void should_track_gradients();
void should_backward_simple_add();
void should_backward_chain();
void should_detach();
void should_zero_grad();

} // namespace my::test::test_autograd

#endif // TEST_AUTOGRAD_HPP
