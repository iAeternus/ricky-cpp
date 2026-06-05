#include "test_array.hpp"
#include "array.hpp"
#include "ricky_test.hpp"

namespace my::test::test_array {

void it_works() {
    util::Array<CString> arr(5);
    Assertions::assert_false(arr.empty());

    arr[0] = "aaa", arr[1] = "bbb", arr[2] = "ccc";
    Assertions::assert_equals(5, arr.len());
    Assertions::assert_equals("[aaa,bbb,ccc,,]"_cs, arr.to_string());

    arr.resize(2);
    Assertions::assert_equals(2, arr.len());
    Assertions::assert_equals("[,]"_cs, arr.to_string());

    arr.resize(0);
    Assertions::assert_true(arr.empty());
    Assertions::assert_equals("[]"_cs, arr.to_string());
}

void should_at() {
    // Given
    util::Array<char> arr = {'a', 'b', 'c'};

    // When
    auto res = arr.at(0);

    // Then
    Assertions::assert_equals('a', res);

    // When
    arr.at(0) = arr.at(1);

    // Then
    Assertions::assert_equals("[b,b,c]"_cs, arr.to_string());
}

GROUP_NAME("test_array")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(it_works),
    UNIT_TEST_ITEM(should_at))

} // namespace my::test::test_array
