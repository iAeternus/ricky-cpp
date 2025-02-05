#ifndef TEST_STRING_HPP
#define TEST_STRING_HPP

#include "ricky_test.hpp"
#include "String.hpp"
#include "Array.hpp"

namespace my::test::test_string {

auto should_construct = []() {
    String s = "abc";
    Assertions::assertEquals("abc"_cs, s.__str__());
};

auto should_add = []() {
    // Given
    String s = "abc"_s;
    String s2 = "bcd"_s;

    // When
    s += s2;

    // Then
    Assertions::assertEquals(6LL, s.size());
    Assertions::assertEquals({'b'}, s[3]);
    Assertions::assertEquals("abcbcd"_s, s);
};

auto should_mul = []() {
    // Given
    String s = "abc"_s;

    // When
    s = s * 2;

    // Then
    Assertions::assertEquals(6, i32(s.size()));
    Assertions::assertEquals({'a'}, s[3]);
};

auto should_split = []() {
    // Given
    String s = "abcdef"_s;

    // When
    auto res = s.split(1, 2);
    auto res2 = s.split(3);
    auto res3 = s.split(1, -1);

    // Then
    Assertions::assertEquals(1, i32(res.size()));
    Assertions::assertEquals("b"_s, res);
    Assertions::assertEquals("def"_s, res2);
    Assertions::assertEquals("bcde"_s, res3);
};

auto should_find = []() {
    // Given
    String s = "abcdeff"_s;

    // When
    auto pos = s.find("def"_s);
    auto pos2 = s.find('f');
    auto pos3 = s.find("abd"_s);

    // Then
    Assertions::assertEquals(3LL, pos);
    Assertions::assertEquals(5LL, pos2);
    Assertions::assertEquals(String::npos, pos3);
};

auto should_find_all = []() {
    // Given
    String s = "abcdefabc"_s;

    // When
    auto poss = s.findAll("abc"_s);

    // Then
    Assertions::assertEquals(2LL, poss.size());
    Assertions::assertEquals("[0,6]"_cs, poss.__str__());
};

auto should_judge_starts_with = []() {
    // Given
    String s = "abcdef"_s;

    // When
    bool res = s.startsWith("abc"_s);
    bool res2 = s.startsWith("abd"_s);

    // Then
    Assertions::assertTrue(res);
    Assertions::assertFalse(res2);
};

auto should_judge_ends_with = []() {
    // Given
    String s = "abcdef"_s;

    // When
    bool res = s.endsWith("def"_s);
    bool res2 = s.endsWith("deg"_s);

    // Then
    Assertions::assertTrue(res);
    Assertions::assertFalse(res2);
};

auto should_get_upper = []() {
    // Given
    String s = "abcDef"_s;

    // When
    auto res = s.upper();

    // Then
    Assertions::assertEquals("ABCDEF"_s, res);
};

auto should_get_lower = []() {
    // Given
    String s = "ABCdEF"_s;

    // When
    auto res = s.lower();

    // Then
    Assertions::assertEquals("abcdef"_s, res);
};

auto should_trim = []() {
    // Given
    String s = "   abcdef   "_s;

    // When
    auto res = s.trim();

    // Then
    Assertions::assertEquals("abcdef"_s, res);
};

auto should_replace = []() {
    // Given
    String s = "abcdefabc"_s;

    // When
    auto res = s.replace("abc", "def");

    // Then
    Assertions::assertEquals("defdefdef"_s, res);
};

auto should_maintain_encoding = []() {
    // Given
    String s = "你好世界";

    // When
    String s2 = s;
    String s3 = s2.split(0);

    // Then
    Assertions::assertEquals(s, s2);
    Assertions::assertEquals(s2, s3);

    // When
    s3[1] = 'a';

    // Then
    Assertions::assertEquals("你好世界"_s, s);
    Assertions::assertEquals("你a世界"_s, s2);
    Assertions::assertEquals("你a世界"_s, s3);
    Assertions::assertEquals("你好世界"_s, s.split(0, s.size()));

    // When
    auto res = s.replace("你"_s, "你们"_s);

    // Then
    Assertions::assertEquals("你们好世界"_s, res);
};

// auto should_join_iterator = []() {
//     // Given
//     util::Array<int> arr = {1, 2, 3, 4, 5};

//     // When
//     String s = ", "_s.join(arr);

//     // Then
//     Assertions::assertEquals("1, 2, 3, 4, 5"_s, s);
// };

auto should_match_parentheses = []() {
    // Given
    String s = "{a, b, c, d, [1, 2, 3], {x: 1, y: 2}}";

    // When
    auto res = s.match('{', '}');
    auto res2 = s.match('[', ']');

    // Then
    Assertions::assertEquals("{a, b, c, d, [1, 2, 3], {x: 1, y: 2}}"_s, res);
    Assertions::assertEquals("[1, 2, 3]"_s, res2);
};

auto should_compare = []() {
    // Given
    String s = "abc";
    String s2 = "abd";
    String s3 = "abcc";
    String s4 = "abc";

    // When
    cmp_t res = s.__cmp__(s2);
    cmp_t res2 = s.__cmp__(s3);
    cmp_t res3 = s2.__cmp__(s3);
    cmp_t res4 = s.__cmp__(s4);

    // Then
    Assertions::assertEquals(-1LL, res);
    Assertions::assertEquals(-1LL, res2);
    Assertions::assertEquals(1LL, res3);
    Assertions::assertEquals(0LL, res4);
};

auto should_remove_all = []() {
    // Given
    String s = "   a  bc ";

    // When
    auto res = s.removeAll(' ');

    // Then
    Assertions::assertEquals("abc"_s, res);
};

void test_string() {
    UnitTestGroup group{"test_string"};

    group.addTest("should_construct", should_construct);
    group.addTest("should_add", should_add);
    group.addTest("should_mul", should_mul);
    group.addTest("should_split", should_split);
    group.addTest("should_find", should_find);
    group.addTest("should_find_all", should_find_all);
    group.addTest("should_judge_starts_with", should_judge_starts_with);
    group.addTest("should_judge_ends_with", should_judge_ends_with);
    group.addTest("should_get_upper", should_get_upper);
    group.addTest("should_get_lower", should_get_lower);
    group.addTest("should_trim", should_trim);
    group.addTest("should_replace", should_replace);
    group.addTest("should_maintain_encoding", should_maintain_encoding);
    // group.addTest("should_join_iterator", should_join_iterator);
    group.addTest("should_match_parentheses", should_match_parentheses);
    group.addTest("should_compare", should_compare);
    group.addTest("should_remove_all", should_remove_all);

    group.startAll();
}

} // namespace my::test::test_string

#endif // TEST_STRING_HPP