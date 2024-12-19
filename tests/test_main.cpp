#include "test_test_utils.hpp"
#include "test_cstring.hpp"
#include "test_dynarray.hpp"
#include "test_chain.hpp"
#include "test_bi_chain.hpp"
#include "test_binary_utils.hpp"
#include "test_dist.hpp"

#include "test_speed.hpp"

using namespace my::test;

#define TEST_SPEED 1

int main() {
    test_test_utils::should_group_unit_test();
    test_cstring::test_cstring();
    test_dynarray::test_dynarray();
    test_chain::test_chain();
    test_bi_chain::test_bi_chain();
    test_binary_utils::test_binary_utils();
    test_dict::test_dict();

#if TEST_SPEED
    test_speed();
#endif

    return 0;
}