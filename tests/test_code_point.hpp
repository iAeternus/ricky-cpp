#ifndef TEST_CODE_POINT_HPP
#define TEST_CODE_POINT_HPP

#include "Encoding.hpp"
#include "ricky_test.hpp"
#include "CodePoint.hpp"
#include "Random.hpp"

namespace my::test::test_code_point {

auto should_compare = []() {
    Assertions::assertTrue(util::CodePoint{'a'} == 'a');
    Assertions::assertFalse(util::CodePoint{'a'} != 'a');
    Assertions::assertTrue('a' == util::CodePoint{'a'});
    Assertions::assertFalse('a' != util::CodePoint{'a'});
};

auto test_code_point() {
    UnitTestGroup group("test_code_point");

    group.addTest("should_compare", should_compare);

    group.startAll();
}

} // namespace my::test::test_code_point

#endif // TEST_CODE_POINT_HPP