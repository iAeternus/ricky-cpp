// #include "test_test_utils.hpp"
#include "test_cstring.hpp"
#include "test_dynarray.hpp"
#include "test_chain.hpp"

using namespace my::test;

int main() {
    // should_group_unit_test();

    test_cstring::test_cstring();
    test_dynarray::test_dynarray();
    test_dynarray::test_dynarray_speed();
    test_chain::test_chain();

    return 0;
}