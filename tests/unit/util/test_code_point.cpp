#include "test_code_point.hpp"
#include "code_point.hpp"
#include "ricky_test.hpp"

namespace my::test::test_code_point {

void should_compare() {
    Assertions::assert_true(util::CodePoint{'a'} == 'a');
    Assertions::assert_false(util::CodePoint{'a'} != 'a');
    Assertions::assert_true('a' == util::CodePoint{'a'});
    Assertions::assert_false('a' != util::CodePoint{'a'});
}

GROUP_NAME("test_code_point")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(should_compare))

} // namespace my::test::test_code_point