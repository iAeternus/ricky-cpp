#include "test_str_string.hpp"
#include "string.hpp"
#include "ricky_test.hpp"

namespace my::test::test_str_string {

using namespace my::str;

namespace {

std::string to_std(StringView view) {
    return std::string(reinterpret_cast<const char*>(view.as_bytes()), view.len());
}

} // namespace

void test_basic_len_and_bytes() {
    String s("hello");
    Assertions::assert_equals(static_cast<usize>(5), s.len());
    Assertions::assert_false(s.is_empty());

    std::string bytes;
    for (auto b : s.bytes()) {
        bytes.push_back(static_cast<char>(b));
    }
    Assertions::assert_equals(std::string("hello"), bytes);
}

void test_empty_basic() {
    String s;
    Assertions::assert_equals(static_cast<usize>(0), s.len());
    Assertions::assert_true(s.is_empty());
    Assertions::assert_equals(std::string(""), to_std(s.as_str()));
}

void test_bytes_range_methods() {
    String s("ab");
    auto b0 = s.bytes().nth(0);
    auto b2 = s.bytes().nth(2);
    auto last = s.bytes().last();

    Assertions::assert_true(b0.is_some());
    Assertions::assert_equals(static_cast<u8>('a'), b0.unwrap());
    Assertions::assert_true(b2.is_none());
    Assertions::assert_true(last.is_some());
    Assertions::assert_equals(static_cast<u8>('b'), last.unwrap());
}

void test_chars_range_methods() {
    // "h" + U+1F600 (grinning face) in UTF-8: F0 9F 98 80
    String s("h\xF0\x9F\x98\x80");
    auto c0 = s.chars().nth(0);
    auto c1 = s.chars().nth(1);
    auto c2 = s.chars().nth(2);
    auto last = s.chars().last();

    Assertions::assert_true(c0.is_some());
    Assertions::assert_equals(static_cast<u32>('h'), static_cast<u32>(c0.unwrap()));
    Assertions::assert_true(c1.is_some());
    Assertions::assert_equals(static_cast<u32>(0x1F600), static_cast<u32>(c1.unwrap()));
    Assertions::assert_true(c2.is_none());
    Assertions::assert_true(last.is_some());
    Assertions::assert_equals(static_cast<u32>(0x1F600), static_cast<u32>(last.unwrap()));
}

void test_chars_bytes_utf8() {
    // "h" + U+1F600 (grinning face) in UTF-8: F0 9F 98 80
    String s("h\xF0\x9F\x98\x80");
    Assertions::assert_equals(static_cast<usize>(5), s.len());
    Assertions::assert_equals(static_cast<usize>(5), s.bytes().count());
    Assertions::assert_equals(static_cast<usize>(5), s.bytes().size());
    Assertions::assert_equals(static_cast<usize>(2), s.chars().count());
    Assertions::assert_equals(static_cast<usize>(2), s.chars().size());

    std::vector<char32_t> chars;
    for (auto c : s.chars()) {
        chars.push_back(c);
    }
    Assertions::assert_equals(static_cast<usize>(2), static_cast<usize>(chars.size()));
    Assertions::assert_equals(static_cast<u32>('h'), static_cast<u32>(chars[0]));
    Assertions::assert_equals(static_cast<u32>(0x1F600), static_cast<u32>(chars[1]));

    String s2("你好");
    Assertions::assert_equals(static_cast<usize>(6), s2.len());
    usize cnt = 0;
    for (auto c : s2.chars()) {
        (void)c;
        ++cnt;
    }
    Assertions::assert_equals(static_cast<usize>(2), cnt);

    usize sum_index = 0;
    for (auto [idx, b] : s.bytes().enumerate()) {
        (void)b;
        sum_index += idx;
    }
    Assertions::assert_equals(static_cast<usize>(10), sum_index);

    usize char_index_sum = 0;
    for (auto [idx, ch] : s.chars().enumerate()) {
        (void)ch;
        char_index_sum += idx;
    }
    Assertions::assert_equals(static_cast<usize>(1), char_index_sum);

    auto b1 = s.bytes().nth(1);
    Assertions::assert_true(b1.is_some());
    Assertions::assert_equals(static_cast<u8>(0xF0), b1.unwrap());

    auto b_last = s.bytes().last();
    Assertions::assert_true(b_last.is_some());
    Assertions::assert_equals(static_cast<u8>(0x80), b_last.unwrap());

    auto c1 = s.chars().nth(1);
    Assertions::assert_true(c1.is_some());
    Assertions::assert_equals(static_cast<u32>(0x1F600), static_cast<u32>(c1.unwrap()));

    auto c_last = s.chars().last();
    Assertions::assert_true(c_last.is_some());
    Assertions::assert_equals(static_cast<u32>(0x1F600), static_cast<u32>(c_last.unwrap()));
}

void test_push_pop() {
    String s;
    s.push(U'a');
    s.push_str(StringView("bc"));
    Assertions::assert_equals(static_cast<usize>(3), s.len());

    auto popped = s.pop();
    Assertions::assert_true(popped.is_some());
    Assertions::assert_equals(static_cast<u32>('c'), static_cast<u32>(popped.unwrap()));
    Assertions::assert_equals(std::string("ab"), to_std(s.as_str()));
}

void test_push_pop_edge_cases() {
    String s;
    auto popped = s.pop();
    Assertions::assert_true(popped.is_none());

    s.push_str(StringView(""));
    Assertions::assert_equals(static_cast<usize>(0), s.len());
}

void test_find_basic() {
    String s("hello world");
    auto pos = s.find(StringView("world"));
    Assertions::assert_true(pos.is_some());
    Assertions::assert_equals(static_cast<usize>(6), pos.unwrap());
}

void test_find_edge_cases() {
    String s("hello world");
    auto pos_empty = s.find(StringView(""));
    Assertions::assert_true(pos_empty.is_some());
    Assertions::assert_equals(static_cast<usize>(0), pos_empty.unwrap());

    auto pos_none = s.find(StringView("xyz"));
    Assertions::assert_true(pos_none.is_none());

    // "h" + U+1F600 (grinning face) in UTF-8: F0 9F 98 80
    String s2("h\xF0\x9F\x98\x80");
    // U+1F600 (grinning face) in UTF-8: F0 9F 98 80
    auto pos2 = s2.find(StringView("\xF0\x9F\x98\x80", 4));
    Assertions::assert_true(pos2.is_some());
    Assertions::assert_equals(static_cast<usize>(1), pos2.unwrap());

    String s3("aaaaa");
    auto pos3 = s3.find(StringView("aaa"));
    Assertions::assert_true(pos3.is_some());
    Assertions::assert_equals(static_cast<usize>(0), pos3.unwrap());
}

void test_contains_starts_ends_edge() {
    String s("hello world");
    Assertions::assert_true(s.contains(StringView("hello")));
    Assertions::assert_true(s.starts_with(StringView("hello")));
    Assertions::assert_true(s.ends_with(StringView("world")));

    Assertions::assert_true(s.starts_with(StringView("")));
    Assertions::assert_true(s.ends_with(StringView("")));
    Assertions::assert_true(s.contains(StringView("")));

    Assertions::assert_false(s.contains(StringView("xyz")));
    Assertions::assert_false(s.starts_with(StringView("world")));
    Assertions::assert_false(s.ends_with(StringView("hello")));
}

void test_strip_prefix_suffix_edge() {
    String s("hello world");
    auto strip1 = s.strip_prefix(StringView("hello "));
    Assertions::assert_true(strip1.is_some());
    Assertions::assert_equals(std::string("world"), to_std(strip1.unwrap()));

    auto strip2 = s.strip_suffix(StringView(" world"));
    Assertions::assert_true(strip2.is_some());
    Assertions::assert_equals(std::string("hello"), to_std(strip2.unwrap()));

    auto strip3 = s.strip_prefix(StringView("nope"));
    Assertions::assert_true(strip3.is_none());
    auto strip4 = s.strip_suffix(StringView("nope"));
    Assertions::assert_true(strip4.is_none());
}

void test_split_basic() {
    String s(" a b  c ");
    auto parts = s.split_whitespace();
    Assertions::assert_equals(static_cast<usize>(3), parts.len());
    Assertions::assert_equals(std::string("a"), to_std(parts.at(0)));
    Assertions::assert_equals(std::string("b"), to_std(parts.at(1)));
    Assertions::assert_equals(std::string("c"), to_std(parts.at(2)));
}

void test_split_empty_pattern() {
    String s("ab");
    auto parts = s.split(StringView(""));
    Assertions::assert_equals(static_cast<usize>(4), parts.len());
    Assertions::assert_equals(std::string(""), to_std(parts.at(0)));
    Assertions::assert_equals(std::string("a"), to_std(parts.at(1)));
    Assertions::assert_equals(std::string("b"), to_std(parts.at(2)));
    Assertions::assert_equals(std::string(""), to_std(parts.at(3)));
}

void test_split_whitespace_edge_cases() {
    String s(" \t\r\n a \t b\n\nc ");
    auto parts = s.split_whitespace();
    Assertions::assert_equals(static_cast<usize>(3), parts.len());
    Assertions::assert_equals(std::string("a"), to_std(parts.at(0)));
    Assertions::assert_equals(std::string("b"), to_std(parts.at(1)));
    Assertions::assert_equals(std::string("c"), to_std(parts.at(2)));
}

void test_lines_edge_cases() {
    String s1("");
    auto lines1 = s1.lines();
    Assertions::assert_equals(static_cast<usize>(0), lines1.len());

    String s2("a\r\nb\nc\n");
    auto lines = s2.lines();
    Assertions::assert_equals(static_cast<usize>(3), lines.len());
    Assertions::assert_equals(std::string("a"), to_std(lines.at(0)));
    Assertions::assert_equals(std::string("b"), to_std(lines.at(1)));
    Assertions::assert_equals(std::string("c"), to_std(lines.at(2)));
}

void test_replace_basic() {
    String s2("abab");
    auto replaced = s2.replace(StringView("ab"), StringView("x"));
    Assertions::assert_equals(std::string("xx"), to_std(replaced.as_str()));
}

void test_replace_edge_cases() {
    String s("aaaa");
    auto r1 = s.replace(StringView("aa"), StringView("b"));
    Assertions::assert_equals(std::string("bb"), to_std(r1.as_str()));

    auto r2 = s.replace(StringView("x"), StringView("y"));
    Assertions::assert_equals(std::string("aaaa"), to_std(r2.as_str()));

    auto r3 = s.replace(StringView("a"), StringView(""));
    Assertions::assert_equals(std::string(""), to_std(r3.as_str()));
}

void test_trim_edge_cases() {
    String s("   ");
    auto trimmed = s.trim().to_string();
    Assertions::assert_equals(std::string(""), to_std(trimmed.as_str()));
}

void test_case_convert_and_into_bytes() {
    String s("AbC");
    auto lower = s.to_lowercase();
    auto upper = s.to_uppercase();
    Assertions::assert_equals(std::string("abc"), to_std(lower.as_str()));
    Assertions::assert_equals(std::string("ABC"), to_std(upper.as_str()));

    String s2("hi");
    auto bytes = std::move(s2).into_bytes();
    Assertions::assert_equals(static_cast<usize>(2), bytes.len());
    Assertions::assert_equals(static_cast<u8>('h'), bytes.at(0));
    Assertions::assert_equals(static_cast<u8>('i'), bytes.at(1));
}

void test_case_convert_non_ascii() {
    // "Hi" + U+1F600 (grinning face) in UTF-8: F0 9F 98 80
    String s("Hi\xF0\x9F\x98\x80");
    auto lower = s.to_lowercase();
    auto upper = s.to_uppercase();
    // "hi" + U+1F600 (grinning face) in UTF-8: F0 9F 98 80
    Assertions::assert_equals(std::string("hi\xF0\x9F\x98\x80"), to_std(lower.as_str()));
    // "HI" + U+1F600 (grinning face) in UTF-8: F0 9F 98 80
    Assertions::assert_equals(std::string("HI\xF0\x9F\x98\x80"), to_std(upper.as_str()));
}

void test_into_bytes_edge_cases() {
    String s;
    auto bytes = std::move(s).into_bytes();
    Assertions::assert_equals(static_cast<usize>(0), bytes.len());
}

void test_reserve_capacity() {
    String s("a");
    s.reserve(128);
    Assertions::assert_true(s.capacity() >= 128);
    s.reserve(8);
    Assertions::assert_true(s.capacity() >= 128);
}

GROUP_NAME("test_str_string");
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(test_basic_len_and_bytes),
    UNIT_TEST_ITEM(test_empty_basic),
    UNIT_TEST_ITEM(test_bytes_range_methods),
    UNIT_TEST_ITEM(test_chars_range_methods),
    UNIT_TEST_ITEM(test_chars_bytes_utf8),
    UNIT_TEST_ITEM(test_push_pop),
    UNIT_TEST_ITEM(test_push_pop_edge_cases),
    UNIT_TEST_ITEM(test_find_basic),
    UNIT_TEST_ITEM(test_find_edge_cases),
    UNIT_TEST_ITEM(test_contains_starts_ends_edge),
    UNIT_TEST_ITEM(test_strip_prefix_suffix_edge),
    UNIT_TEST_ITEM(test_split_basic),
    UNIT_TEST_ITEM(test_split_empty_pattern),
    UNIT_TEST_ITEM(test_split_whitespace_edge_cases),
    UNIT_TEST_ITEM(test_lines_edge_cases),
    UNIT_TEST_ITEM(test_replace_basic),
    UNIT_TEST_ITEM(test_replace_edge_cases),
    UNIT_TEST_ITEM(test_trim_edge_cases),
    UNIT_TEST_ITEM(test_case_convert_and_into_bytes),
    UNIT_TEST_ITEM(test_case_convert_non_ascii),
    UNIT_TEST_ITEM(test_into_bytes_edge_cases),
    UNIT_TEST_ITEM(test_reserve_capacity));

} // namespace my::test::test_str_string
