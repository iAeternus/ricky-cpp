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

auto should_slice = []() {
    // Given
    CString s = "abcdef";

    // When
    auto res = s.slice(1, 2);
    auto res2 = s.slice(3);
    auto res3 = s.slice(1, -1);

    // Then
    Assertions::assertEquals(1, res.length());
    Assertions::assertEquals("b"_cs, res.to_string());
    Assertions::assertEquals("def"_cs, res2.to_string());
    Assertions::assertEquals("bcde"_cs, res3.to_string());
};

auto should_find = []() {
    // Given
    CString s = "abcdeff";
    CString s2 = "caabaabaabaabaaaab";

    // When
    auto pos = s.find("def");
    auto pos2 = s.find('f');
    auto pos3 = s.find("abd");
    auto pos4 = s2.find("aabaabaaaa");
    auto pos5 = s2.find("");

    // Then
    Assertions::assertEquals(3, pos);
    Assertions::assertEquals(5, pos2);
    Assertions::assertEquals(npos, pos3);
    Assertions::assertEquals(7, pos4);
    Assertions::assertEquals(npos, pos5);
};

auto should_find_all = []() {
    // Given
    CString s = "abcdefabc";

    // When
    auto poss = s.find_all("abc");

    // Then
    Assertions::assertEquals(2, poss.size());
    Assertions::assertEquals(0, poss[0]);
    Assertions::assertEquals(6, poss[1]);
};

auto should_judge_starts_with = []() {
    // Given
    CString s = "abcdef";

    // When
    bool res = s.starts_with("abc");
    bool res2 = s.starts_with("abd");

    // Then
    Assertions::assertTrue(res);
    Assertions::assertFalse(res2);
};

auto should_judge_ends_with = []() {
    // Given
    CString s = "abcdef";

    // When
    bool res = s.ends_with("def");
    bool res2 = s.ends_with("deg");

    // Then
    Assertions::assertTrue(res);
    Assertions::assertFalse(res2);
};

auto should_get_upper = []() {
    // Given
    CString s = "abcDef";

    // When
    auto res = s.upper();

    // Then
    Assertions::assertEquals("ABCDEF"_cs, res);
};

auto should_get_lower = []() {
    // Given
    CString s = "ABCdEF";

    // When
    auto res = s.lower();

    // Then
    Assertions::assertEquals("abcdef"_cs, res);
};

auto should_trim = []() {
    // Given
    CString s = "   abcdef   ";

    // When
    auto res = s.trim();

    // Then
    Assertions::assertEquals("abcdef"_cs, res.to_string());
};

auto should_remove_all = []() {
    // Given
    CString s = "   a  bc "_cs;

    // When
    auto res = s.remove_all(' ');
    auto res2 = s.remove_all([](const auto& ch) {
        return ch == ' ';
    });

    // Then
    Assertions::assertEquals("abc"_cs, res);
    Assertions::assertEquals("abc"_cs, res2);
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

auto should_iterate = []() {
    CString str = "abcdefg";
    util::Vec<char> chs;

    // When
    for (auto&& c : str) {
        c++;
    }

    for (const auto& c : str) {
        chs.append(c);
    }

    // Then
    Assertions::assertEquals("bcdefgh"_cs, str.__str__());
    Assertions::assertEquals("[b,c,d,e,f,g,h]"_cs, chs.__str__());
};

auto test_cstring_view = []() {
    // Given
    CString str = "abcdefg";
    util::Vec<char> chs;

    // When
    const CStringView sv = str.slice(1, 6);
    for (const char c : sv) {
        chs.append(c);
    }

    // Then
    Assertions::assertEquals("bcdef"_cs, sv.to_string());
    Assertions::assertEquals('b', sv[0]);
    Assertions::assertEquals('f', sv[sv.length() - 1]);
    Assertions::assertEquals("[b,c,d,e,f]"_cs, chs.__str__());
};

auto test_cstring() {
    UnitTestGroup group{"test_cstring"};

    group.addTest("should_construct", should_construct);
    group.addTest("should_hash", should_hash);
    group.addTest("should_compare", should_compare);
    group.addTest("should_slice", should_slice);
    group.addTest("should_find", should_find);
    group.addTest("should_find_all", should_find_all);
    group.addTest("should_judge_starts_with", should_judge_starts_with);
    group.addTest("should_judge_ends_with", should_judge_ends_with);
    group.addTest("should_get_upper", should_get_upper);
    group.addTest("should_get_lower", should_get_lower);
    group.addTest("should_trim", should_trim);
    group.addTest("should_remove_all", should_remove_all);
    group.addTest("should_add", should_add);
    group.addTest("should_iterate", should_iterate);
    group.addTest("test_cstring_view", test_cstring_view);

    group.startAll();
}

} // namespace my::test::test_cstring

#endif // TEST_CSTRING_HPP