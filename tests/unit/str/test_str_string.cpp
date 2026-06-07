#include "test_str_string.hpp"
#include "string.hpp"
#include "vec.hpp"
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

void test_from_numeric() {
    auto s1 = String::from_i32(42);
    Assertions::assert_equals(std::string("42"), to_std(s1.as_str()));

    auto s2 = String::from_i32(-42);
    Assertions::assert_equals(std::string("-42"), to_std(s2.as_str()));

    auto s3 = String::from_u32(42);
    Assertions::assert_equals(std::string("42"), to_std(s3.as_str()));

    auto s4 = String::from_i64(1234567890123LL);
    Assertions::assert_equals(std::string("1234567890123"), to_std(s4.as_str()));

    auto s5 = String::from_u64(1234567890123ULL);
    Assertions::assert_equals(std::string("1234567890123"), to_std(s5.as_str()));

    auto s6 = String::from_f64(3.14);
    Assertions::assert_false(s6.is_empty());
}

void test_operator_add() {
    String a("hello");
    StringView b(" world");
    auto c = a + b;
    Assertions::assert_equals(std::string("hello world"), to_std(c.as_str()));

    auto d = a + String(" world");
    Assertions::assert_equals(std::string("hello world"), to_std(d.as_str()));

    auto e = String("hello ") + "world";
    Assertions::assert_equals(std::string("hello world"), to_std(e.as_str()));

    auto f = "hello " + String("world");
    Assertions::assert_equals(std::string("hello world"), to_std(f.as_str()));
}

void test_operator_mul() {
    String s("ab");
    auto r = s * 3;
    Assertions::assert_equals(std::string("ababab"), to_std(r.as_str()));

    auto r2 = s * 0;
    Assertions::assert_true(r2.is_empty());

    auto r3 = s.repeat(2);
    Assertions::assert_equals(std::string("abab"), to_std(r3.as_str()));
}

void test_match_indices() {
    String s("abcabcabc");
    auto indices = s.match_indices(StringView("abc"));
    Assertions::assert_equals(static_cast<usize>(3), indices.len());
    Assertions::assert_equals(static_cast<usize>(0), indices.at(0));
    Assertions::assert_equals(static_cast<usize>(3), indices.at(1));
    Assertions::assert_equals(static_cast<usize>(6), indices.at(2));

    auto indices2 = s.match_indices(StringView("xyz"));
    Assertions::assert_equals(static_cast<usize>(0), indices2.len());

    auto indices3 = s.match_indices(StringView(""));
    Assertions::assert_equals(static_cast<usize>(0), indices3.len());
}

void test_string_trim_start_end() {
    String s("  hello  ");
    auto trimmed_start = s.trim_start();
    Assertions::assert_equals(std::string("hello  "), to_std(trimmed_start));

    auto trimmed_end = s.trim_end();
    Assertions::assert_equals(std::string("  hello"), to_std(trimmed_end));

    String s2("");
    Assertions::assert_true(s2.trim_start().is_empty());
    Assertions::assert_true(s2.trim_end().is_empty());

    String s3("hello");
    Assertions::assert_equals(std::string("hello"), to_std(s3.trim_start()));
    Assertions::assert_equals(std::string("hello"), to_std(s3.trim_end()));
}

void test_trim_matches() {
    String s("ababXabab");
    auto t1 = s.trim_matches(StringView("ab"));
    Assertions::assert_equals(std::string("X"), to_std(t1));

    auto t2 = s.trim_start_matches(StringView("ab"));
    Assertions::assert_equals(std::string("Xabab"), to_std(t2));

    auto t3 = s.trim_end_matches(StringView("ab"));
    Assertions::assert_equals(std::string("ababX"), to_std(t3));

    String s2("hello");
    auto t4 = s2.trim_matches(StringView("xyz"));
    Assertions::assert_equals(std::string("hello"), to_std(t4));

    auto t5 = s2.trim_matches(StringView(""));
    Assertions::assert_equals(std::string("hello"), to_std(t5));
}

void test_join() {
    String sep(", ");
    util::Vec<String> items;
    items.push(String("a"));
    items.push(String("b"));
    items.push(String("c"));
    auto joined = sep.join(items);
    Assertions::assert_equals(std::string("a, b, c"), to_std(joined.as_str()));

    util::Vec<String> empty;
    auto joined2 = sep.join(empty);
    Assertions::assert_true(joined2.is_empty());

    util::Vec<i32> vec;
    vec.push(1);
    vec.push(2);
    vec.push(3);
    auto joined3 = sep.join(vec);
    Assertions::assert_equals(std::string("1, 2, 3"), to_std(joined3.as_str()));
}

void test_match_brackets() {
    String s("{a, b, [1, 2], {x: 1}}");
    auto m1 = s.match_brackets(U'{', U'}');
    Assertions::assert_equals(std::string("{a, b, [1, 2], {x: 1}}"), to_std(m1.as_str()));

    auto m2 = s.match_brackets(U'[', U']');
    Assertions::assert_equals(std::string("[1, 2]"), to_std(m2.as_str()));

    String s2("no brackets");
    auto m3 = s2.match_brackets(U'{', U'}');
    Assertions::assert_true(m3.is_empty());

    String s3("{unmatched");
    Assertions::assert_throws<Exception>("Unmatched brackets, too many left brackets", [&]() {
        s3.match_brackets(U'{', U'}');
    });
}

void test_remove_all() {
    String s("aabbaa");
    auto r1 = s.remove_all(U'a');
    Assertions::assert_equals(std::string("bb"), to_std(r1.as_str()));

    auto r2 = s.remove_all([](char32_t cp) { return cp == U'b'; });
    Assertions::assert_equals(std::string("aaaa"), to_std(r2.as_str()));
}

void test_swap() {
    String a("hello");
    String b("world");
    a.swap(b);
    Assertions::assert_equals(std::string("world"), to_std(a.as_str()));
    Assertions::assert_equals(std::string("hello"), to_std(b.as_str()));
}

void test_push_format() {
    String s;
    s.push_format("Case {}: {}+{}={}", 1, 1, 1, 2);
    Assertions::assert_equals(std::string("Case 1: 1+1=2"), to_std(s.as_str()));
}

void test_push_n() {
    String s;
    s.push_n(U'a', 5);
    Assertions::assert_equals(std::string("aaaaa"), to_std(s.as_str()));
}

void test_shrink_to_fit() {
    String s("hello");
    s.reserve(128);
    Assertions::assert_true(s.capacity() >= 128);
    s.shrink_to_fit();
    Assertions::assert_true(s.capacity() < 128);
}

void test_slice_negative() {
    String s("hello");
    auto sv = s.slice(static_cast<usize>(1), static_cast<isize>(-1));
    Assertions::assert_equals(std::string("ell"), to_std(sv));
}

void test_split_max_split() {
    String s("a,b,c,d");
    auto parts = s.split(StringView(","), 2);
    Assertions::assert_equals(static_cast<usize>(3), parts.len());
    Assertions::assert_equals(std::string("a"), to_std(parts.at(0)));
    Assertions::assert_equals(std::string("b"), to_std(parts.at(1)));
    Assertions::assert_equals(std::string("c,d"), to_std(parts.at(2)));
}

void test_string_iterators() {
    String s("abc");
    usize count = 0;
    for (auto it = s.begin(); it != s.end(); ++it) {
        ++count;
    }
    Assertions::assert_equals(static_cast<usize>(3), count);

    u8 sum = 0;
    for (auto it = s.begin(); it != s.end(); ++it) {
        sum += *it;
    }
    Assertions::assert_equals(static_cast<u8>('a' + 'b' + 'c'), sum);
}

void test_at_out_of_range() {
    String s("abc");
    Assertions::assert_throws("Index 100 out of bounds [0..3]", [&]() {
        volatile auto x = s.at(100);
        (void)x;
    });
}

void test_comparison_operators() {
    String a("abc");
    String b("abc");
    String c("abd");

    Assertions::assert_true(a == b);
    Assertions::assert_false(a != b);
    Assertions::assert_true(a != c);
    Assertions::assert_true(a < c);
    Assertions::assert_true(a <= b);
    Assertions::assert_true(c > a);
    Assertions::assert_true(c >= b);
}

void test_copy_and_move() {
    String a("hello");
    String b = a;
    Assertions::assert_equals(std::string("hello"), to_std(b.as_str()));

    String c = std::move(a);
    Assertions::assert_equals(std::string("hello"), to_std(c.as_str()));

    String d;
    d = b;
    Assertions::assert_equals(std::string("hello"), to_std(d.as_str()));
}

void test_split_pattern() {
    String s("a,b,c");
    auto parts = s.split(StringView(","));
    Assertions::assert_equals(static_cast<usize>(3), parts.len());
    Assertions::assert_equals(std::string("a"), to_std(parts.at(0)));
    Assertions::assert_equals(std::string("b"), to_std(parts.at(1)));
    Assertions::assert_equals(std::string("c"), to_std(parts.at(2)));

    String s2("a.b.c");
    auto parts2 = s2.split(StringView("."));
    Assertions::assert_equals(static_cast<usize>(3), parts2.len());
}

void test_string_view_trim() {
    StringView sv("  hello  ");
    auto t1 = sv.trim_start();
    Assertions::assert_equals(std::string("hello  "), to_std(t1));

    auto t2 = sv.trim_end();
    Assertions::assert_equals(std::string("  hello"), to_std(t2));

    auto t3 = sv.trim();
    Assertions::assert_equals(std::string("hello"), to_std(t3));

    StringView sv2("ababXabab");
    auto t4 = sv2.trim_matches(StringView("ab"));
    Assertions::assert_equals(std::string("X"), to_std(t4));

    auto t5 = sv2.trim_start_matches(StringView("ab"));
    Assertions::assert_equals(std::string("Xabab"), to_std(t5));

    auto t6 = sv2.trim_end_matches(StringView("ab"));
    Assertions::assert_equals(std::string("ababX"), to_std(t6));
}

void test_string_view_match_indices() {
    StringView sv("abcabcabc");
    auto indices = sv.match_indices(StringView("abc"));
    Assertions::assert_equals(static_cast<usize>(3), indices.len());
    Assertions::assert_equals(static_cast<usize>(0), indices.at(0));
    Assertions::assert_equals(static_cast<usize>(3), indices.at(1));
    Assertions::assert_equals(static_cast<usize>(6), indices.at(2));
}

void test_string_view_split_max() {
    StringView sv("a,b,c,d");
    auto parts = sv.split(StringView(","), 2);
    Assertions::assert_equals(static_cast<usize>(3), parts.len());
    Assertions::assert_equals(std::string("a"), to_std(parts.at(0)));
    Assertions::assert_equals(std::string("b"), to_std(parts.at(1)));
    Assertions::assert_equals(std::string("c,d"), to_std(parts.at(2)));
}

void test_string_view_iterators() {
    StringView sv("abc");
    usize count = 0;
    for (auto it = sv.begin(); it != sv.end(); ++it) {
        ++count;
    }
    Assertions::assert_equals(static_cast<usize>(3), count);

    u8 sum = 0;
    for (auto it = sv.begin(); it != sv.end(); ++it) {
        sum += *it;
    }
    Assertions::assert_equals(static_cast<u8>('a' + 'b' + 'c'), sum);
}

void test_string_view_to_numeric() {
    StringView sv("42");
    Assertions::assert_equals(static_cast<i64>(42), sv.to_i64());
    Assertions::assert_equals(static_cast<f64>(42.0), sv.to_f64());

    StringView sv2("3.14");
    auto f = sv2.to_f64();
    Assertions::assert_true(f > 3.0 && f < 4.0);

    StringView sv3("");
    Assertions::assert_equals(static_cast<i64>(0), sv3.to_i64());

    StringView sv4("abc");
    Assertions::assert_equals(static_cast<i64>(0), sv4.to_i64());
}

void test_retain() {
    String s("a1b2c3");
    s.retain([](char32_t cp) { return cp >= U'a' && cp <= U'z'; });
    Assertions::assert_equals(std::string("abc"), to_std(s.as_str()));
}

void test_string_trim_no_whitespace() {
    String s("hello");
    auto t = s.trim();
    Assertions::assert_equals(std::string("hello"), to_std(t));

    auto ts = s.trim_start();
    Assertions::assert_equals(std::string("hello"), to_std(ts));

    auto te = s.trim_end();
    Assertions::assert_equals(std::string("hello"), to_std(te));
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
    UNIT_TEST_ITEM(test_reserve_capacity),
    UNIT_TEST_ITEM(test_from_numeric),
    UNIT_TEST_ITEM(test_operator_add),
    UNIT_TEST_ITEM(test_operator_mul),
    UNIT_TEST_ITEM(test_match_indices),
    UNIT_TEST_ITEM(test_string_trim_start_end),
    UNIT_TEST_ITEM(test_trim_matches),
    UNIT_TEST_ITEM(test_join),
    UNIT_TEST_ITEM(test_match_brackets),
    UNIT_TEST_ITEM(test_remove_all),
    UNIT_TEST_ITEM(test_swap),
    UNIT_TEST_ITEM(test_push_format),
    UNIT_TEST_ITEM(test_push_n),
    UNIT_TEST_ITEM(test_shrink_to_fit),
    UNIT_TEST_ITEM(test_slice_negative),
    UNIT_TEST_ITEM(test_split_max_split),
    UNIT_TEST_ITEM(test_string_iterators),
    UNIT_TEST_ITEM(test_at_out_of_range),
    UNIT_TEST_ITEM(test_comparison_operators),
    UNIT_TEST_ITEM(test_copy_and_move),
    UNIT_TEST_ITEM(test_split_pattern),
    UNIT_TEST_ITEM(test_string_view_trim),
    UNIT_TEST_ITEM(test_string_view_match_indices),
    UNIT_TEST_ITEM(test_string_view_split_max),
    UNIT_TEST_ITEM(test_string_view_iterators),
    UNIT_TEST_ITEM(test_string_view_to_numeric),
    UNIT_TEST_ITEM(test_retain),
    UNIT_TEST_ITEM(test_string_trim_no_whitespace));

} // namespace my::test::test_str_string
