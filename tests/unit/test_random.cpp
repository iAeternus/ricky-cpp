#include "unit/test_random.hpp"

#include "unit_test.hpp"
#include "random.hpp"
#include "printer.hpp"

#include "test/test_registry.hpp"

namespace my::test::test_random {

void should_generate_random() {
    io::println(std::format("u8: {}", util::Random::instance().next<u8>()));
    io::println(std::format("u16: {}", util::Random::instance().next<u16>()));
    io::println(std::format("u32: {}", util::Random::instance().next<u32>()));
    io::println(std::format("u64: {}", util::Random::instance().next<u64>()));
    io::println(std::format("i8: {}", util::Random::instance().next<i8>()));
    io::println(std::format("i16: {}", util::Random::instance().next<i16>()));
    io::println(std::format("i32: {}", util::Random::instance().next<i32>()));
    io::println(std::format("i64: {}", util::Random::instance().next<i64>()));
    io::println(std::format("f32: {}", util::Random::instance().next<f32>()));
    io::println(std::format("f64: {}", util::Random::instance().next<f64>()));
    io::println(std::format("CString: {}", util::Random::instance().next_str(6)));
}

void should_generate_uniform_sum_numbers() {
    // Given
    i32 n = 100, sum = 1e6;

    // When & Then
    io::println(util::Random::instance().generate_uniform_sum_numbers(n, sum));
}

void test_random() {
    UnitTestGroup group{"test_random"};

    group.addTest("should_generate_random", should_generate_random);
    group.addTest("should_generate_uniform_sum_numbers", should_generate_uniform_sum_numbers);

    group.startAll();
}

GROUP_NAME("test_random")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(should_generate_random),
    UNIT_TEST_ITEM(should_generate_uniform_sum_numbers))
} // namespace my::test::test_random