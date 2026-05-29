
#ifndef TEST_RANDOM_HPP
#define TEST_RANDOM_HPP

namespace my::test::test_random {

void should_generate_integer_random();
void should_generate_float_random();
void should_generate_range_random();
void should_generate_normal_random();
void should_generate_bernoulli_random();
void should_generate_random_string();
void should_shuffle_sequence();
void should_partition_nonnegative();
void should_seed_random();

} // namespace my::test::test_random

#endif // TEST_RANDOM_HPP
