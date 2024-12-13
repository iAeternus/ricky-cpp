// #include "test_test_utils.hpp"
// #include "test_ricky.hpp"
#include "test_cstring.hpp"
#include "test_dynarray.hpp"

int main() {
    // should_group_unit_test();
    // test_ricky();

    test_cstring();
    test_dynarray();
    test_dynarray_speed();

    return 0;
}