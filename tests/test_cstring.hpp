#ifndef TEST_CSTRING_HPP
#define TEST_CSTRING_HPP

#include "ricky_test.hpp"

using namespace my;

auto should_construct = []() {
    // Given
    CString str = "abc";

    // When & Then
    test::Assertions::assertEquals(3, int(str.size()));
    test::Assertions::assertFalse(str.empty());
    test::Assertions::assertEquals('a', str[0]);
    test::Assertions::assertEquals('b', str[1]);
    test::Assertions::assertEquals('c', str[2]);
};

auto should_hash = []() {
    // Given
    CString str1 = "abc";
    CString str2 = "abd";

    // When
    auto hash1 = str1.__hash__();
    auto hash2 = str2.__hash__();

    // Then
    test::Assertions::assertNotEquals(hash1, hash2);
};

auto should_compare = []() {
    // Given
    CString str1 = "aaa";
    CString str2 = "aaaa";
    CString str3 = "aaab";
    CString str4 = "aaab";

    // When & Then
    test::Assertions::assertTrue(str1.__cmp__(str2) < 0);
    test::Assertions::assertTrue(str2.__cmp__(str3) < 0);
    test::Assertions::assertTrue(str3.__cmp__(str4) == 0);

    test::Assertions::assertEquals(str3, str4);
    test::Assertions::assertNotEquals(str2, str4);
};

void test_cstring() {
    test::UnitTestGroup group("test_cstring");

    group.addTest("should_construct", should_construct);
    group.addTest("should_hash", should_hash);
    group.addTest("should_compare", should_compare);

    group.startAll();
}

#endif // TEST_CSTRING_HPP