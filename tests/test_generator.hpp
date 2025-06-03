#ifndef TEST_GENERATOR_HPP
#define TEST_GENERATOR_HPP

#include "UnitTest.hpp"
#include "Assertions.hpp"
#include "Generator.hpp"
#include "DynArray.hpp"

namespace my::test::test_generator {

my::coro::Generator<i32> range(i32 start, i32 end) {
    for (i32 i = start; i <= end; ++i) {
        co_yield i;
    }
}

fn should_generate = []() {
    // Given
    util::DynArray<i32> res;

    // When
    for (const auto& item : range(1, 5)) {
        res.append(item);
    }

    // Then
    Assertions::assertEquals("[1,2,3,4,5]"_cs, res.__str__());
};

fn test_generator() {
    UnitTestGroup group{"test_generator"};

    group.addTest("should_generate", should_generate);

    group.startAll();
}

} // namespace my::test::test_generator

#endif // TEST_GENERATOR_HPP