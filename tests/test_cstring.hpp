#ifndef TEST_CSTRING_HPP
#define TEST_CSTRING_HPP

#include "ricky_test.hpp"

namespace my::test::test_cstring {

auto should_construct = []() {
    // Given
    CString str = "abc";

    // When & Then
    Assertions::assertEquals(3, i32(str.size()));
    Assertions::assertFalse(str.empty());
    Assertions::assertEquals('a', str[0]);
    Assertions::assertEquals('b', str[1]);
    Assertions::assertEquals('c', str[2]);
};

auto should_hash = []() {
    // Given
    CString str1 = "abc";
    CString str2 = "abd";

    // When
    auto hash1 = str1.__hash__();
    auto hash2 = str2.__hash__();

    // Then
    Assertions::assertNotEquals(hash1, hash2);
};

auto should_compare = []() {
    // Given
    CString str1 = "aaa";
    CString str2 = "aaaa";
    CString str3 = "aaab";
    CString str4 = "aaab";

    // When & Then
    Assertions::assertTrue(str1.__cmp__(str2) < 0);
    Assertions::assertTrue(str2.__cmp__(str3) < 0);
    Assertions::assertTrue(str3.__cmp__(str4) == 0);

    Assertions::assertEquals(str3, str4);
    Assertions::assertNotEquals(str2, str4);
};

auto should_add = []() {
    // Given
    CString str = "aaa";
    CString str2 = "bbb";

    // When
    auto res = str + str2;

    // Then
    Assertions::assertEquals("aaabbb"_cs, res);
};

void test_cstring() {
    UnitTestGroup group{"test_cstring"};

    group.addTest("should_construct", should_construct);
    group.addTest("should_hash", should_hash);
    group.addTest("should_compare", should_compare);
    group.addTest("should_add", should_add);

    group.startAll();
}

} // namespace my::test::test_cstring

#endif // TEST_CSTRING_HPP