#include "test_cstring.hpp"
#include "ricky_test.hpp"

namespace my::test::test_cstring {

void should_construct() {
    // Given
    CString str = "abc";

    // When & Then
    Assertions::assert_equals(3, i32(str.size()));
    Assertions::assert_false(str.empty());
    Assertions::assert_equals('a', str[0]);
    Assertions::assert_equals('b', str[1]);
    Assertions::assert_equals('c', str[2]);
}

void should_hash() {
    // Given
    CString str1 = "abc";
    CString str2 = "abd";

    // When
    auto hash1 = str1.hash();
    auto hash2 = str2.hash();

    // Then
    Assertions::assert_not_equals(hash1, hash2);
}

void should_compare() {
    // Given
    CString str1 = "aaa";
    CString str2 = "aaaa";
    CString str3 = "aaab";
    CString str4 = "aaab";

    // When & Then
    Assertions::assert_true(str1.cmp(str2) < 0);
    Assertions::assert_true(str2.cmp(str3) < 0);
    Assertions::assert_true(str3.cmp(str4) == 0);

    Assertions::assert_equals(str3, str4);
    Assertions::assert_not_equals(str2, str4);
}

void should_slice() {
    // Given
    CString s = "abcdef";

    // When
    auto res = s.slice(1, 2);
    auto res2 = s.slice(3);
    auto res3 = s.slice(1, -1);

    // Then
    Assertions::assert_equals(1ULL, res.length());
    Assertions::assert_equals("b"_cs, res.to_string());
    Assertions::assert_equals("def"_cs, res2.to_string());
    Assertions::assert_equals("bcde"_cs, res3.to_string());
}

void should_find() {
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
    Assertions::assert_equals(3ULL, pos);
    Assertions::assert_equals(5ULL, pos2);
    Assertions::assert_equals(npos, pos3);
    Assertions::assert_equals(7ULL, pos4);
    Assertions::assert_equals(npos, pos5);
}

void should_find_all() {
    // Given
    CString s = "abcdefabc";

    // When
    auto poss = s.find_all("abc");

    // Then
    Assertions::assert_equals(2ULL, poss.size());
    Assertions::assert_equals(0ULL, poss[0]);
    Assertions::assert_equals(6ULL, poss[1]);
}

void should_judge_starts_with() {
    // Given
    CString s = "abcdef";

    // When
    bool res = s.starts_with("abc");
    bool res2 = s.starts_with("abd");

    // Then
    Assertions::assert_true(res);
    Assertions::assert_false(res2);
}

void should_judge_ends_with() {
    // Given
    CString s = "abcdef";

    // When
    bool res = s.ends_with("def");
    bool res2 = s.ends_with("deg");

    // Then
    Assertions::assert_true(res);
    Assertions::assert_false(res2);
}

void should_get_upper() {
    // Given
    CString s = "abcDef";

    // When
    auto res = s.upper();

    // Then
    Assertions::assert_equals("ABCDEF"_cs, res);
}

void should_get_lower() {
    // Given
    CString s = "ABCdEF";

    // When
    auto res = s.lower();

    // Then
    Assertions::assert_equals("abcdef"_cs, res);
}

void should_trim() {
    // Given
    CString s = "   abcdef   ";

    // When
    auto res = s.trim();

    // Then
    Assertions::assert_equals("abcdef"_cs, res.to_string());
}

void should_remove_all() {
    // Given
    CString s = "   a  bc "_cs;

    // When
    auto res = s.remove_all(' ');
    auto res2 = s.remove_all([](const auto& ch) {
        return ch == ' ';
    });

    // Then
    Assertions::assert_equals("abc"_cs, res);
    Assertions::assert_equals("abc"_cs, res2);
}

void should_add() {
    // Given
    CString str = "aaa";
    CString str2 = "bbb";

    // When
    auto res = str + str2;

    // Then
    Assertions::assert_equals("aaabbb"_cs, res);
}

void should_iterate() {
    CString str = "abcdefg";
    util::Vec<char> chs;

    // When
    for (auto&& c : str) {
        c++;
    }

    for (const auto& c : str) {
        chs.push(c);
    }

    // Then
    Assertions::assert_equals("bcdefgh"_cs, str.to_string());
    Assertions::assert_equals("[b,c,d,e,f,g,h]"_cs, chs.to_string());
}

void test_cstring_view() {
    // Given
    CString str = "abcdefg";
    util::Vec<char> chs;

    // When
    const CStringView sv = str.slice(1, 6);
    for (const char c : sv) {
        chs.push(c);
    }

    // Then
    Assertions::assert_equals("bcdef"_cs, sv.to_string());
    Assertions::assert_equals('b', sv[0]);
    Assertions::assert_equals('f', sv[sv.length() - 1]);
    Assertions::assert_equals("[b,c,d,e,f]"_cs, chs.to_string());
}

GROUP_NAME("test_cstring")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(should_construct),
    UNIT_TEST_ITEM(should_hash),
    UNIT_TEST_ITEM(should_compare),
    UNIT_TEST_ITEM(should_slice),
    UNIT_TEST_ITEM(should_find),
    UNIT_TEST_ITEM(should_find_all),
    UNIT_TEST_ITEM(should_judge_starts_with),
    UNIT_TEST_ITEM(should_judge_ends_with),
    UNIT_TEST_ITEM(should_get_upper),
    UNIT_TEST_ITEM(should_get_lower),
    UNIT_TEST_ITEM(should_trim),
    UNIT_TEST_ITEM(should_remove_all),
    UNIT_TEST_ITEM(should_add),
    UNIT_TEST_ITEM(should_iterate),
    UNIT_TEST_ITEM(test_cstring_view))

} // namespace my::test::test_cstring
