#ifndef TEST_ARRAY_HPP
#define TEST_ARRAY_HPP

#include "ricky_test.hpp"
#include "Array.hpp"

namespace my::test::test_array {

fn it_works = []() {
    util::Array<CString> arr(5);
    Assertions::assertFalse(arr.empty());

    arr[0] = "aaa", arr[1] = "bbb", arr[2] = "ccc";
    Assertions::assertEquals(5, arr.size());
    Assertions::assertEquals("[aaa,bbb,ccc,,]"_cs, arr.__str__());

    arr.resize(2);
    Assertions::assertEquals(2, arr.size());
    Assertions::assertEquals("[,]"_cs, arr.__str__());

    arr.resize(0);
    Assertions::assertTrue(arr.empty());
    Assertions::assertEquals("[]"_cs, arr.__str__());
};

fn should_at = []() {
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
};

fn test_array() {
    UnitTestGroup group{"test_array"};

    group.addTest("it_works", it_works);
    group.addTest("should_at", should_at);

    group.startAll();
}

} // namespace my::test::test_array

#endif // TEST_ARRAY_HPP