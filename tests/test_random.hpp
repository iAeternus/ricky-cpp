#ifndef TEST_RANDOM_HPP
#define TEST_RANDOM_HPP

#include "ricky_test.hpp"
#include "Random.hpp"
#include "printer.hpp"

namespace my::test::test_random {

auto should_generate_random = []() {
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
    io::println(std::format("CString: {}", util::Random::instance().nextStr(6)));
};

void test_random() {
    UnitTestGroup group{"test_random"};

    group.addTest("should_generate_random", should_generate_random);

    group.startAll();
}

} // namespace my::test::test_random

#endif // TEST_RANDOM_HPP