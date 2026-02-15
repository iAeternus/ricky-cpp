#include "test_generator.hpp"
#include "generator.hpp"
#include "vec.hpp"
#include "ricky_test.hpp"

namespace my::test::test_generator {

static auto range(i32 start, i32 end) -> my::coro::Generator<i32> {
    for (i32 i = start; i <= end; ++i) {
        co_yield i;
    } // TODO: 没有return?
}

void should_generate() {
    // Given
    util::Vec<i32> res;

    // When
    for (const auto& item : range(1, 5)) {
        res.push(item);
    }

    // Then
    Assertions::assertEquals("[1,2,3,4,5]"_cs, res.__str__());
}

GROUP_NAME("test_generator")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(should_generate))

} // namespace my::test::test_generator