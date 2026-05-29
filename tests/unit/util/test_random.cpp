#include "test_random.hpp"

#include "random.hpp"
#include "ricky_test.hpp"

namespace my::test::test_random {

using util::Random;

void should_generate_integer_random() {
    // Given
    auto& rng = Random::thread_local_rng();

    // When
    const auto u8_value = rng.gen<u8>();
    const auto u16_value = rng.gen<u16>();
    const auto u32_value = rng.gen<u32>();
    const auto u64_value = rng.gen<u64>();

    const auto i8_value = rng.gen<i8>();
    const auto i16_value = rng.gen<i16>();
    const auto i32_value = rng.gen<i32>();
    const auto i64_value = rng.gen<i64>();

    // Then
    Assertions::assert_true(u8_value <= std::numeric_limits<u8>::max());
    Assertions::assert_true(u16_value <= std::numeric_limits<u16>::max());
    Assertions::assert_true(u32_value <= std::numeric_limits<u32>::max());
    Assertions::assert_true(u64_value <= std::numeric_limits<u64>::max());
    Assertions::assert_true(i8_value >= std::numeric_limits<i8>::min());
    Assertions::assert_true(i8_value <= std::numeric_limits<i8>::max());
    Assertions::assert_true(i16_value >= std::numeric_limits<i16>::min());
    Assertions::assert_true(i16_value <= std::numeric_limits<i16>::max());
    Assertions::assert_true(i32_value >= std::numeric_limits<i32>::min());
    Assertions::assert_true(i32_value <= std::numeric_limits<i32>::max());
    Assertions::assert_true(i64_value >= std::numeric_limits<i64>::min());
    Assertions::assert_true(i64_value <= std::numeric_limits<i64>::max());
}

void should_generate_float_random() {
    // Given
    auto& rng = Random::thread_local_rng();

    // When
    const f32 f32_value = rng.gen<f32>();
    const f64 f64_value = rng.gen<f64>();

    // Then
    Assertions::assert_true(f32_value >= 0.0f);
    Assertions::assert_true(f32_value < 1.0f);
    Assertions::assert_true(f64_value >= 0.0);
    Assertions::assert_true(f64_value < 1.0);
}

void should_generate_range_random() {
    // Given
    auto& rng = Random::thread_local_rng();

    // When
    const i32 int_value = rng.gen_range<i32>(10, 20);
    const f64 float_value = rng.gen_range<f64>(1.5, 3.5);

    // Then
    Assertions::assert_true(int_value >= 10);
    Assertions::assert_true(int_value < 20);
    Assertions::assert_true(float_value >= 1.5);
    Assertions::assert_true(float_value < 3.5);
}

void should_generate_normal_random() {
    // Given
    auto& rng = Random::thread_local_rng();

    // When
    const auto value = rng.normal<f64>(0.0, 1.0);

    // Then
    Assertions::assert_false(std::isnan(value)); // 正态分布无法精确断言，这里只验证：不为NaN
}

void should_generate_bernoulli_random() {
    // Given
    auto& rng = Random::thread_local_rng();

    // When
    const bool value = rng.bernoulli(0.5);

    // Then
    Assertions::assert_true(value == true || value == false);
}

void should_generate_random_string() {
    // Given
    auto& rng = Random::thread_local_rng();

    // When
    const auto str = rng.string(32);

    // Then
    Assertions::assert_equals(static_cast<usize>(32), str.size());
    for (const auto ch : str) {
        const bool valid = ('a' <= ch && ch <= 'z')
                           || ('A' <= ch && ch <= 'Z')
                           || ('0' <= ch && ch <= '9');
        Assertions::assert_true(valid);
    }
}

void should_shuffle_sequence() {
    // Given
    auto& rng = Random::thread_local_rng();
    util::Vec<i32> values{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto original = values;

    // When
    rng.shuffle(values.begin(), values.end());

    // Then
    Assertions::assert_equals(original.len(), values.len());

    /*
     * 排序后应完全相同。
     */
    std::sort(values.begin(), values.end());
    std::sort(original.begin(), original.end());
    Assertions::assert_equals(original, values);
}

void should_partition_nonnegative() {
    // Given
    auto& rng = Random::thread_local_rng();
    constexpr i32 n = 100;
    constexpr i32 sum = 100000;

    // When
    const auto result = rng.partition_nonnegative(n, sum);

    // Then
    Assertions::assert_equals(static_cast<usize>(n), result.len());
    i32 actual_sum = 0;
    for (const auto v : result) {
        Assertions::assert_true(v >= 0);
        actual_sum += v;
    }
    Assertions::assert_equals(sum, actual_sum);
}

void should_seed_random() {
    // Given
    Random rng1(42);
    Random rng2(42);

    // When
    const auto a1 = rng1.gen<u32>();
    const auto a2 = rng1.gen<u32>();

    const auto b1 = rng2.gen<u32>();
    const auto b2 = rng2.gen<u32>();

    // Then
    Assertions::assert_equals(a1, b1);

    Assertions::assert_equals(a2, b2);
}

GROUP_NAME("test_random")

REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(should_generate_integer_random),
    UNIT_TEST_ITEM(should_generate_float_random),
    UNIT_TEST_ITEM(should_generate_range_random),
    UNIT_TEST_ITEM(should_generate_normal_random),
    UNIT_TEST_ITEM(should_generate_bernoulli_random),
    UNIT_TEST_ITEM(should_generate_random_string),
    UNIT_TEST_ITEM(should_shuffle_sequence),
    UNIT_TEST_ITEM(should_partition_nonnegative),
    UNIT_TEST_ITEM(should_seed_random))

} // namespace my::test::test_random
