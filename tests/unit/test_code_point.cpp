#include "unit/test_code_point.hpp"

#include "encoding.hpp"
#include "ricky_test.hpp"
#include "code_point.hpp"
#include "random.hpp"

#include "test/test_registry.hpp"

namespace my::test::test_code_point {

void should_compare() {
    Assertions::assertTrue(util::CodePoint{'a'} == 'a');
    Assertions::assertFalse(util::CodePoint{'a'} != 'a');
    Assertions::assertTrue('a' == util::CodePoint{'a'});
    Assertions::assertFalse('a' != util::CodePoint{'a'});
}

void test_code_point() {
    UnitTestGroup group("test_code_point");

    group.addTest("should_compare", should_compare);

    group.startAll();
}

GROUP_NAME("test_code_point")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(should_compare))
} // namespace my::test::test_code_point