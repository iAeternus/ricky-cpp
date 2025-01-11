#ifndef TEST_STRING_HPP
#define TEST_STRING_HPP

#include "ricky_test.hpp"
#include "String.hpp"
#include "Array.hpp"

namespace my::test::test_string {

using namespace my::util;

auto should_add = []() {
    // Given
    String s = "abc"_s;
    String s2 = "bcd"_s;

    // When
    s += s2;

    // Then
    Assertions::assertEquals(6, i32(s.size()));
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
    Assertions::assertEquals(util::String::npos, pos3);
};

auto should_find_all = []() {
    // Given
    String s = "abcdefabc"_s;

    // When
    auto poss = s.findAll("abc"_s);

    // Then
    Assertions::assertEquals(2, i32(poss.size()));
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
    util::String s = "你好世界";

    // When
    util::String s2 = s;
    util::String s3 = s2.split(0);

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
//     util::String s = ", "_s.join(arr);

//     // Then
//     Assertions::assertEquals("1, 2, 3, 4, 5"_s, s);
// };

auto should_match_parentheses = []() {
    // Given
    util::String s = "{a, b, c, d, [1, 2, 3], {x: 1, y: 2}}";

    // When
    auto res = s.match('{', '}');
    auto res2 = s.match('[', ']');

    // Then
    Assertions::assertEquals("{a, b, c, d, [1, 2, 3], {x: 1, y: 2}}"_s, res);
    Assertions::assertEquals("[1, 2, 3]"_s, res2);
};

void test_string() {
    UnitTestGroup group{"test_string"};

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

    group.startAll();
}

} // namespace my::test::test_string

#endif // TEST_STRING_HPP