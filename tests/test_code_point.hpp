#ifndef TEST_CODE_POINT_HPP
#define TEST_CODE_POINT_HPP

#include "ricky_test.hpp"
#include "CodePoint.hpp"
#include "Random.hpp"

namespace my::test::test_code_point {

constexpr i32 N = 1e6;

auto speed_of_code_point_pool_create = []() {
    util::DynArray<util::CodePoint> cps;
    for(auto i = 0; i < N; ++i) {
        cps.append(*(util::CodePointPool::instance().get(util::Random::instance().nextStr(1), util::encoding_map(util::UTF8))));
    }
    Assertions::assertEquals(N, cps.size());
};

auto speed_of_original_code_point_create = []() {
    util::DynArray<util::CodePoint> cps;
    for(auto i = 0; i < N; ++i) {
        cps.append(util::CodePoint(util::Random::instance().nextStr(1), util::encoding_map(util::UTF8)));
    }
    Assertions::assertEquals(N, cps.size());
};

void test_code_point_pool_speed() {
    UnitTestGroup group{"test_code_point_pool_speed"};

    group.addTest("speed_of_code_point_pool_create", speed_of_code_point_pool_create);
    group.addTest("speed_of_original_code_point_create", speed_of_original_code_point_create);

    group.startAll();
}

} // namespace my::test::test_code_point

#endif // TEST_CODE_POINT_HPP