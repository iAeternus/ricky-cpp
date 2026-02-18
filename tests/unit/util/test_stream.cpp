#include "test_stream.hpp"
#include "stream.hpp"
#include "my_pair.hpp"
#include "ricky_test.hpp"

namespace my::test::test_stream {

void should_operates() {
    // Given
    util::Vec<i32> d = {1, 2, 3, 4, 5, 6, 7, 8, 9};

    // When
    auto res = util::stream(d)
                   .filter([](const auto& item) { return item % 2 == 0; })
                   .map([](const auto& item) { return item * 2; })
                   .collect();

    // Then
    Assertions::assertEquals("[4,8,12,16]"_cs, res.__str__());
}

void should_map_objects() {
    // Given
    util::Vec<Pair<i32, i32>> pairs = {{1, 1}, {2, 2}, {3, 3}};

    // When
    auto res = util::stream(pairs)
                   .map([](const auto& item) { return item.first(); })
                   .collect();

    // When
    Assertions::assertEquals("[1,2,3]"_cs, res.__str__());
}

GROUP_NAME("test_stream")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(should_operates),
    UNIT_TEST_ITEM(should_map_objects))

} // namespace my::test::test_stream