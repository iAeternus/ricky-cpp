#ifndef TEST_GENERATOR_HPP
#define TEST_GENERATOR_HPP

#include "my_types.hpp"

#include "my_types.hpp"

namespace my::coro {
template <typename T>
class Generator;
} // namespace my::coro

namespace my::test::test_generator {
auto range(i32 start, i32 end) -> my::coro::Generator<i32>;
void should_generate();
void test_generator();
} // namespace my::test::test_generator

#endif // TEST_GENERATOR_HPP