#include "unit/test_string.hpp"

#include "ricky_test.hpp"
#include "str.hpp"
#include "array.hpp"
#include "vec.hpp"

#include "test/test_registry.hpp"

namespace my::test::test_string {

void should_construct() {
    util::String s = "abc";
    Assertions::assertEquals("abc"_cs, s.__str__());
}

void should_add() {
    // Given
    util::String s = "abc";
    util::String s2 = "bcd";
    CString cs = "cde";

    // When
    s += s2;
    s += cs;

    // Then
    Assertions::assertEquals(9ULL, s.len());
    Assertions::assertEquals(util::CodePoint{'b'}, s[3]);
    Assertions::assertEquals("abcbcdcde"_s, s);
}

void should_mul() {
    // Given
    util::String s = "abc";

    // When
    s = s * 2;

    // Then
    Assertions::assertEquals(6ULL, s.len());
    Assertions::assertEquals(util::CodePoint{'a'}, s[3]);
}

void should_slice() {
    // Given
    util::String s = "abcdef";

    // When
    auto res = s.slice(1, 2);
    auto res2 = s.slice(3);
    auto res3 = s.slice(1, -1);

    // Then
    Assertions::assertEquals(1ULL, res.len());
    Assertions::assertEquals("b"_s, res);
    Assertions::assertEquals("def"_s, res2);
    Assertions::assertEquals("bcde"_s, res3);
}

void should_find() {
    // Given
    util::String s = "abcdeff";
    util::String s2 = "caabaabaabaabaaaab";

    // When
    auto pos = s.find("def"_s);
    auto pos2 = s.find(util::CodePoint('f'));
    auto pos3 = s.find("abd"_s);
    auto pos4 = s2.find("aabaabaaaa"_s);
    auto pos5 = s2.find(""_s);

    // Then
    Assertions::assertEquals(3ULL, pos);
    Assertions::assertEquals(5ULL, pos2);
    Assertions::assertEquals(npos, pos3);
    Assertions::assertEquals(7ULL, pos4);
    Assertions::assertEquals(npos, pos5);
}

void should_find_all() {
    // Given
    util::String s = "abcdefabc";

    // When
    auto poss = s.find_all("abc"_s);

    // Then
    Assertions::assertEquals(2ULL, poss.len());
    Assertions::assertEquals("[0,6]"_cs, poss.__str__());
}

void should_judge_starts_with() {
    // Given
    util::String s = "abcdef";

    // When
    bool res = s.starts_with("abc"_s);
    bool res2 = s.starts_with("abd"_s);

    // Then
    Assertions::assertTrue(res);
    Assertions::assertFalse(res2);
}

void should_judge_ends_with() {
    // Given
    util::String s = "abcdef";

    // When
    bool res = s.ends_with("def"_s);
    bool res2 = s.ends_with("deg"_s);

    // Then
    Assertions::assertTrue(res);
    Assertions::assertFalse(res2);
}

void should_get_upper() {
    // Given
    util::String s = "abcDef";

    // When
    auto res = s.upper();

    // Then
    Assertions::assertEquals("ABCDEF"_s, res);
}

void should_get_lower() {
    // Given
    util::String s = "ABCdEF";

    // When
    auto res = s.lower();

    // Then
    Assertions::assertEquals("abcdef"_s, res);
}

void should_trim() {
    // Given
    util::String s = "   abcdef   ";

    // When
    auto res = s.trim();

    // Then
    Assertions::assertEquals("abcdef"_s, res);
}

void should_replace() {
    // Given
    util::String s = "abcdefabc";

    // When
    auto res = s.replace("abc"_s, "def"_s);

    // Then
    Assertions::assertEquals("defdefdef"_s, res);
}

void should_maintain_encoding() {
    // Given
    util::String s = "你好世界";

    // When
    util::String s2 = s;
    util::String s3 = s2.slice(0);

    // Then
    Assertions::assertEquals(s, s2);
    Assertions::assertEquals(s2, s3);

    // When
    s3[1] = 'a';

    // Then
    Assertions::assertEquals("你好世界"_s, s);
    Assertions::assertEquals("你好世界"_s, s2);
    Assertions::assertEquals("你a世界"_s, s3);
    Assertions::assertEquals("你好世界"_s, s.slice(0, s.len()));

    // When
    auto res = s.replace("你"_s, "你们"_s);

    // Then
    Assertions::assertEquals("你们好世界"_s, res);
}

void should_join_iterator() {
    // Given
    util::Array<i32> arr = {1, 2, 3, 4, 5};
    util::Vec<CString> vec = {"aaa", "bbb", "ccc"};

    // When
    auto res = ", "_s.join(arr);
    auto res2 = ", "_s.join(vec);

    // Then
    Assertions::assertEquals("1, 2, 3, 4, 5"_s, res);
    Assertions::assertEquals("aaa, bbb, ccc"_s, res2);
}

void should_match_parentheses() {
    // Given
    util::String s = "{a, b, c, d, [1, 2, 3], {x: 1, y: 2}}";
    util::String s2 = "a]";

    // When
    auto res = s.match('{', '}');
    auto res2 = s.match('[', ']');
    auto res3 = s2.match('[', ']');

    // Then
    Assertions::assertEquals("{a, b, c, d, [1, 2, 3], {x: 1, y: 2}}"_s, res);
    Assertions::assertEquals("[1, 2, 3]"_s, res2);
    Assertions::assertEquals(""_s, res3);
}

void should_fail_match_if_str_invalid() {
    // Given
    util::String s = "{a";

    // When & Then
    Assertions::assertThrows("Unmatched parentheses, too many left parentheses", [&]() {
        auto _ = s.match('{', '}');
    });
}

void should_split() {
    Assertions::assertEquals("[a,b,c]"_cs, "a/b/c"_s.split("/"_s).__str__());
    Assertions::assertEquals("[,,a,b]"_cs, "//a/b"_s.split("/"_s).__str__());
    Assertions::assertEquals("[a,,b]"_cs, "a//b"_s.split("/"_s).__str__());
    Assertions::assertEquals("[a,b,,]"_cs, "a/b//"_s.split("/"_s).__str__());
    Assertions::assertEquals("[a,b,c/d]"_cs, "a/b/c/d"_s.split("/"_s, 2).__str__());
}

void should_compare() {
    // Given
    util::String s = "abc"_s;
    util::String s2 = "abd"_s;
    util::String s3 = "abcc"_s;
    util::String s4 = "abc"_s;

    // When
    cmp_t res = s.__cmp__(s2);
    cmp_t res2 = s.__cmp__(s3);
    cmp_t res3 = s2.__cmp__(s3);
    cmp_t res4 = s.__cmp__(s4);

    // Then
    Assertions::assertEquals(-1, res);
    Assertions::assertEquals(-1, res2);
    Assertions::assertEquals(1, res3);
    Assertions::assertEquals(0, res4);
}

void should_remove_all() {
    // Given
    util::String s = "   a  bc "_s;

    // When
    auto res = s.remove_all(' ');
    auto res2 = s.remove_all([](const auto& cp) {
        return cp == ' ';
    });

    // Then
    Assertions::assertEquals("abc"_s, res);
    Assertions::assertEquals("abc"_s, res2);
}

void test_string_view() {
    // Given
    util::String s = "abc我def";

    // When
    util::StringView sv(s, 1ULL, 5ULL);

    // Then
    Assertions::assertEquals(5ULL, sv.length());
    Assertions::assertEquals(util::CodePoint{'b'}, sv[0]);
    Assertions::assertEquals(util::CodePoint{'e'}, sv[4]);
    Assertions::assertEquals(util::CodePoint{"我"}, sv[2]);
    Assertions::assertEquals("bc我de"_s, sv.to_string());
}

void should_string_view_compare() {
    // Given
    util::String s = "abc_abd_abcc";
    util::StringView s1(s, 0, 3);
    util::StringView s2(s, 4, 3);
    util::StringView s3(s, 8, 4);
    util::StringView s4(s, 0, 3);

    // When
    cmp_t res = s1.__cmp__(s2);
    cmp_t res2 = s1.__cmp__(s3);
    cmp_t res3 = s2.__cmp__(s3);
    cmp_t res4 = s1.__cmp__(s4);

    // Then
    Assertions::assertEquals(-1, res);
    Assertions::assertEquals(-1, res2);
    Assertions::assertEquals(1, res3);
    Assertions::assertEquals(0, res4);
}

void test_string() {
    UnitTestGroup group{"test_string"};

    group.addTest("should_construct", should_construct);
    group.addTest("should_add", should_add);
    group.addTest("should_mul", should_mul);
    group.addTest("should_slice", should_slice);
    group.addTest("should_find", should_find);
    group.addTest("should_find_all", should_find_all);
    group.addTest("should_judge_starts_with", should_judge_starts_with);
    group.addTest("should_judge_ends_with", should_judge_ends_with);
    group.addTest("should_get_upper", should_get_upper);
    group.addTest("should_get_lower", should_get_lower);
    group.addTest("should_trim", should_trim);
    group.addTest("should_replace", should_replace);
    group.addTest("should_maintain_encoding", should_maintain_encoding);
    group.addTest("should_join_iterator", should_join_iterator);
    group.addTest("should_match_parentheses", should_match_parentheses);
    group.addTest("should_fail_match_if_str_invalid", should_fail_match_if_str_invalid);
    group.addTest("should_split", should_split);
    group.addTest("should_compare", should_compare);
    group.addTest("should_remove_all", should_remove_all);
    group.addTest("test_string_view", test_string_view);
    group.addTest("should_string_view_compare", should_string_view_compare);

    group.startAll();
}

GROUP_NAME("test_string")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(should_construct),
    UNIT_TEST_ITEM(should_add),
    UNIT_TEST_ITEM(should_mul),
    UNIT_TEST_ITEM(should_slice),
    UNIT_TEST_ITEM(should_find),
    UNIT_TEST_ITEM(should_find_all),
    UNIT_TEST_ITEM(should_judge_starts_with),
    UNIT_TEST_ITEM(should_judge_ends_with),
    UNIT_TEST_ITEM(should_get_upper),
    UNIT_TEST_ITEM(should_get_lower),
    UNIT_TEST_ITEM(should_trim),
    UNIT_TEST_ITEM(should_replace),
    UNIT_TEST_ITEM(should_maintain_encoding),
    UNIT_TEST_ITEM(should_join_iterator),
    UNIT_TEST_ITEM(should_match_parentheses),
    UNIT_TEST_ITEM(should_fail_match_if_str_invalid),
    UNIT_TEST_ITEM(should_split),
    UNIT_TEST_ITEM(should_compare),
    UNIT_TEST_ITEM(should_remove_all),
    UNIT_TEST_ITEM(test_string_view),
    UNIT_TEST_ITEM(should_string_view_compare))
} // namespace my::test::test_string