#ifndef TEST_CODE_POINT_HPP
#define TEST_CODE_POINT_HPP

#include "Encoding.hpp"
#include "UnitTest.hpp"
#include "Assertions.hpp"
#include "CodePoint.hpp"
#include "Random.hpp"

namespace my::test::test_code_point {

fn should_compare = []() {
    Assertions::assertTrue(util::CodePoint{'a'} == 'a');
    Assertions::assertFalse(util::CodePoint{'a'} != 'a');
    Assertions::assertTrue('a' == util::CodePoint{'a'});
    Assertions::assertFalse('a' != util::CodePoint{'a'});
};

fn test_code_point() {
    UnitTestGroup group("test_code_point");

    group.addTest("should_compare", should_compare);

    group.startAll();
}

constexpr i32 N = 1e6;

fn speed_of_code_point_pool_create = []() {
    util::Vec<util::CodePoint> cps;
    for (usize i = 0; i < N; ++i) {
        cps.append(*(util::CodePointPool::instance().get(util::Random::instance().next_str(1), util::encoding_map(util::EncodingType::UTF8))));
    }
    Assertions::assertEquals(N, cps.size());
};

fn speed_of_original_code_point_create = []() {
    util::Vec<util::CodePoint> cps;
    for (usize i = 0; i < N; ++i) {
        cps.append(util::CodePoint(util::Random::instance().next_str(1), util::encoding_map(util::EncodingType::UTF8)));
    }
    Assertions::assertEquals(N, cps.size());
};

fn test_code_point_pool_speed() {
    UnitTestGroup group{"test_code_point_pool_speed"};

    group.addTest("speed_of_code_point_pool_create", speed_of_code_point_pool_create);
    group.addTest("speed_of_original_code_point_create", speed_of_original_code_point_create);

    group.startAll();
}

} // namespace my::test::test_code_point

#endif // TEST_CODE_POINT_HPP