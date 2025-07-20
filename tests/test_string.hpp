#ifndef TEST_STRING_HPP
#define TEST_STRING_HPP

#include "UnitTest.hpp"
#include "Assertions.hpp"
#include "String.hpp"
#include "Array.hpp"
#include "Vec.hpp"

namespace my::test::test_string {

fn should_construct = []() {
    util::String s = "abc";
    Assertions::assertEquals("abc"_cs, s.__str__());
};

fn should_add = []() {
    // Given
    util::String s = "abc";
    util::String s2 = "bcd";
    CString cs = "cde";

    // When
    s += s2;
    s += cs;

    // Then
    Assertions::assertEquals(9, s.size());
    Assertions::assertEquals(util::CodePoint{'b'}, s[3]);
    Assertions::assertEquals("abcbcdcde"_s, s);
};

fn should_mul = []() {
    // Given
    util::String s = "abc";

    // When
    s = s * 2;

    // Then
    Assertions::assertEquals(6, s.size());
    Assertions::assertEquals(util::CodePoint{'a'}, s[3]);
};

fn should_slice = []() {
    // Given
    util::String s = "abcdef";

    // When
    auto res = s.slice(1, 2);
    auto res2 = s.slice(3);
    auto res3 = s.slice(1, -1);

    // Then
    Assertions::assertEquals(1, res.size());
    Assertions::assertEquals("b"_s, res);
    Assertions::assertEquals("def"_s, res2);
    Assertions::assertEquals("bcde"_s, res3);
};

fn should_find = []() {
    // Given
    util::String s = "abcdeff";
    util::String s2 = "caabaabaabaabaaaab";

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

fn should_find_all = []() {
    // Given
    util::String s = "abcdefabc";

    // When
    auto poss = s.find_all("abc");

    // Then
    Assertions::assertEquals(2, poss.size());
    Assertions::assertEquals("[0,6]"_cs, poss.__str__());
};

fn should_judge_starts_with = []() {
    // Given
    util::String s = "abcdef";

    // When
    bool res = s.starts_with("abc");
    bool res2 = s.starts_with("abd");

    // Then
    Assertions::assertTrue(res);
    Assertions::assertFalse(res2);
};

fn should_judge_ends_with = []() {
    // Given
    util::String s = "abcdef";

    // When
    bool res = s.ends_with("def");
    bool res2 = s.ends_with("deg");

    // Then
    Assertions::assertTrue(res);
    Assertions::assertFalse(res2);
};

fn should_get_upper = []() {
    // Given
    util::String s = "abcDef";

    // When
    auto res = s.upper();

    // Then
    Assertions::assertEquals("ABCDEF"_s, res);
};

fn should_get_lower = []() {
    // Given
    util::String s = "ABCdEF";

    // When
    auto res = s.lower();

    // Then
    Assertions::assertEquals("abcdef"_s, res);
};

fn should_trim = []() {
    // Given
    util::String s = "   abcdef   ";

    // When
    auto res = s.trim();

    // Then
    Assertions::assertEquals("abcdef"_s, res);
};

fn should_replace = []() {
    // Given
    util::String s = "abcdefabc";

    // When
    auto res = s.replace("abc", "def");

    // Then
    Assertions::assertEquals("defdefdef"_s, res);
};

fn should_maintain_encoding = []() {
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
    Assertions::assertEquals("你好世界"_s, s.slice(0, s.size()));

    // When
    auto res = s.replace("你", "你们");

    // Then
    Assertions::assertEquals("你们好世界"_s, res);
};

fn should_join_iterator = []() {
    // Given
    util::Array<i32> arr = {1, 2, 3, 4, 5};
    util::Vec<CString> vec = {"aaa", "bbb", "ccc"};

    // When
    auto res = ", "_s.join(arr);
    auto res2 = ", "_s.join(vec);

    // Then
    Assertions::assertEquals("1, 2, 3, 4, 5"_s, res);
    Assertions::assertEquals("aaa, bbb, ccc"_s, res2);
};

fn should_match_parentheses = []() {
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
};

fn should_fail_match_if_str_invalid = []() {
    // Given
    util::String s = "{a";

    // When & Then
    Assertions::assertThrows("unmatched parentheses, too many left parentheses", [&]() {
        s.match('{', '}');
    });
};

fn should_split = []() {
    Assertions::assertEquals("[a,b,c]"_cs, "a/b/c"_s.split("/").__str__());
    Assertions::assertEquals("[,,a,b]"_cs, "//a/b"_s.split("/").__str__());
    Assertions::assertEquals("[a,,b]"_cs, "a//b"_s.split("/").__str__());
    Assertions::assertEquals("[a,b,,]"_cs, "a/b//"_s.split("/").__str__());
    Assertions::assertEquals("[a,b,c/d]"_cs, "a/b/c/d"_s.split("/", 2).__str__());
};

fn should_compare = []() {
    // Given
    util::String s = "abc";
    util::String s2 = "abd";
    util::String s3 = "abcc";
    util::String s4 = "abc";

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
};

fn should_remove_all = []() {
    // Given
    util::String s = "   a  bc ";

    // When
    auto res = s.remove_all(' ');
    auto res2 = s.remove_all([](const auto& cp) {
        return cp == ' ';
    });

    // Then
    Assertions::assertEquals("abc"_s, res);
    Assertions::assertEquals("abc"_s, res2);
};

fn test_string() {
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

    group.startAll();
}

} // namespace my::test::test_string

#endif // TEST_STRING_HPP