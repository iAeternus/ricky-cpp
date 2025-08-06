#ifndef TEST_GENERATOR_HPP
#define TEST_GENERATOR_HPP

#include "ricky_test.hpp"
#include "Generator.hpp"
#include "Vec.hpp"

namespace my::test::test_generator {

fn range(i32 start, i32 end) -> my::coro::Generator<i32> {
    for (i32 i = start; i <= end; ++i) {
        co_yield i;
    }
}

fn should_generate = []() {
    // Given
    util::Vec<i32> res;

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