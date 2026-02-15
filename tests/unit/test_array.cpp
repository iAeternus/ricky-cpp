#include "unit/test_array.hpp"

#include "ricky_test.hpp"
#include "array.hpp"

#include "test/test_registry.hpp"

namespace my::test::test_array {

void it_works() {
    util::Array<CString> arr(5);
    Assertions::assertFalse(arr.empty());

    arr[0] = "aaa", arr[1] = "bbb", arr[2] = "ccc";
    Assertions::assertEquals(5, arr.len());
    Assertions::assertEquals("[aaa,bbb,ccc,,]"_cs, arr.__str__());

    arr.resize(2);
    Assertions::assertEquals(2, arr.len());
    Assertions::assertEquals("[,]"_cs, arr.__str__());

    arr.resize(0);
    Assertions::assertTrue(arr.empty());
    Assertions::assertEquals("[]"_cs, arr.__str__());
}

void should_at() {
    // Given
    util::Array<char> arr = {'a', 'b', 'c'};

    // When
    auto res = arr.at(0);

    // Then
    Assertions::assertEquals('a', res);

    // When
    arr.at(0) = arr.at(1);

    // Then
    Assertions::assertEquals("[b,b,c]"_cs, arr.__str__());
}

void test_array() {
    UnitTestGroup group{"test_array"};

    group.addTest("it_works", it_works);
    group.addTest("should_at", should_at);

    group.startAll();
}

GROUP_NAME("test_array")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(it_works),
    UNIT_TEST_ITEM(should_at))
} // namespace my::test::test_array