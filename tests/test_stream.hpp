#ifndef TEST_STREAM_HPP
#define TEST_STREAM_HPP

#include "ricky_test.hpp"
#include "stream.hpp"
#include "my_pair.hpp"

namespace my::test::test_stream {

auto should_operates = []() {
    // Given
    util::Vec<i32> d = {1, 2, 3, 4, 5, 6, 7, 8, 9};

    // When
    auto res = util::stream(d)
                   .filter([](const auto& item) { return item % 2 == 0; })
                   .map([](const auto& item) { return item * 2; })
                   .collect();

    // Then
    Assertions::assertEquals("[4,8,12,16]"_cs, res.__str__());
};

auto should_map_objects = []() {
    // Given
    util::Vec<Pair<i32, i32>> pairs = {{1, 1}, {2, 2}, {3, 3}};

    // When
    auto res = util::stream(pairs)
                   .map([](const auto& item) { return item.first(); })
                   .collect();

    // When
    Assertions::assertEquals("[1,2,3]"_cs, res.__str__());
};

auto test_stream() {
    UnitTestGroup group{"test_stream"};

    group.addTest("should_operates", should_operates);
    group.addTest("should_map_objects", should_map_objects);

    group.startAll();
}

} // namespace my::test::test_stream

#endif // TEST_STREAM_HPP