#include "test_code_point.hpp"
#include "code_point.hpp"
#include "ricky_test.hpp"

namespace my::test::test_code_point {

void should_compare() {
    Assertions::assertTrue(util::CodePoint{'a'} == 'a');
    Assertions::assertFalse(util::CodePoint{'a'} != 'a');
    Assertions::assertTrue('a' == util::CodePoint{'a'});
    Assertions::assertFalse('a' != util::CodePoint{'a'});
}

GROUP_NAME("test_code_point")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(should_compare))

} // namespace my::test::test_code_point