#ifndef TEST_RANDOM_HPP
#define TEST_RANDOM_HPP

#include "ricky_test.hpp"
#include "Random.hpp"
#include "printer.hpp"

namespace my::test::test_random {

auto should_generate_random = []() {
    io::println(std::format("u8: {}", util::rnd.nextU8()));
    io::println(std::format("u16: {}", util::rnd.nextU16()));
    io::println(std::format("u32: {}", util::rnd.nextU32()));
    io::println(std::format("u64: {}", util::rnd.nextU64()));
    io::println(std::format("i8: {}", util::rnd.nextI8()));
    io::println(std::format("i16: {}", util::rnd.nextI16()));
    io::println(std::format("i32: {}", util::rnd.nextI32()));
    io::println(std::format("i64: {}", util::rnd.nextI64()));
    io::println(std::format("f32: {}", util::rnd.nextF32(0, 1)));
    io::println(std::format("f64: {}", util::rnd.nextF64(0, 1)));
    io::println(std::format("CString: {}", util::rnd.next(6)));
};

void test_random() {
    UnitTestGroup group{"test_random"};

    group.addTest("should_generate_random", should_generate_random);

    group.startAll();
}

} // namespace my::test::test_random

#endif // TEST_RANDOM_HPP