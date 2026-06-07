#include "test_string_algorithm.hpp"
#include "string_algorithm.hpp"
#include "ricky_test.hpp"

namespace my::test::test_string_algorithm {

using namespace my::str;

void test_twoway_find_basic() {
    auto hay = reinterpret_cast<const u8*>("hello world");
    auto pat = reinterpret_cast<const u8*>("world");
    auto res = twoway_find(hay, 11, pat, 5);
    Assertions::assert_true(res.is_some());
    Assertions::assert_equals(static_cast<usize>(6), res.unwrap());

    auto res_start = twoway_find(hay, 11, reinterpret_cast<const u8*>("hello"), 5);
    Assertions::assert_true(res_start.is_some());
    Assertions::assert_equals(static_cast<usize>(0), res_start.unwrap());

    auto res_end = twoway_find(hay, 11, reinterpret_cast<const u8*>("orld"), 4);
    Assertions::assert_true(res_end.is_some());
    Assertions::assert_equals(static_cast<usize>(7), res_end.unwrap());
}

void test_twoway_find_no_match() {
    auto hay = reinterpret_cast<const u8*>("hello world");
    auto res = twoway_find(hay, 11, reinterpret_cast<const u8*>("xyz"), 3);
    Assertions::assert_true(res.is_none());

    auto res2 = twoway_find(hay, 11, reinterpret_cast<const u8*>("hello world!"), 12);
    Assertions::assert_true(res2.is_none());
}

void test_twoway_find_empty_pattern() {
    auto hay = reinterpret_cast<const u8*>("hello");
    auto res = twoway_find(hay, 5, reinterpret_cast<const u8*>(""), 0);
    Assertions::assert_true(res.is_some());
    Assertions::assert_equals(static_cast<usize>(0), res.unwrap());
}

void test_twoway_find_pattern_longer_than_text() {
    auto hay = reinterpret_cast<const u8*>("ab");
    auto res = twoway_find(hay, 2, reinterpret_cast<const u8*>("abc"), 3);
    Assertions::assert_true(res.is_none());
}

void test_twoway_find_single_char() {
    auto hay = reinterpret_cast<const u8*>("hello");
    auto res = twoway_find(hay, 5, reinterpret_cast<const u8*>("l"), 1);
    Assertions::assert_true(res.is_some());
    Assertions::assert_equals(static_cast<usize>(2), res.unwrap());

    auto res_none = twoway_find(hay, 5, reinterpret_cast<const u8*>("x"), 1);
    Assertions::assert_true(res_none.is_none());
}

void test_twoway_find_repeated_chars() {
    auto hay = reinterpret_cast<const u8*>("aaaaa");

    auto res = twoway_find(hay, 5, reinterpret_cast<const u8*>("aaa"), 3);
    Assertions::assert_true(res.is_some());
    Assertions::assert_equals(static_cast<usize>(0), res.unwrap());

    auto res_mid = twoway_find(reinterpret_cast<const u8*>("aabcabc"), 7, reinterpret_cast<const u8*>("abc"), 3);
    Assertions::assert_true(res_mid.is_some());
    Assertions::assert_equals(static_cast<usize>(1), res_mid.unwrap());

    auto res_mid2 = twoway_find(reinterpret_cast<const u8*>("xabcdeyabcde"), 12, reinterpret_cast<const u8*>("abcde"), 5);
    Assertions::assert_true(res_mid2.is_some());
    Assertions::assert_equals(static_cast<usize>(1), res_mid2.unwrap());
}

void test_twoway_find_periodic_pattern() {
    auto hay = reinterpret_cast<const u8*>("abcabcabcabc");
    auto pat = reinterpret_cast<const u8*>("abcabc");
    auto res = twoway_find(hay, 12, pat, 6);
    Assertions::assert_true(res.is_some());
    Assertions::assert_equals(static_cast<usize>(0), res.unwrap());

    auto res_mid = twoway_find(reinterpret_cast<const u8*>("xxabcabcxx"), 10, pat, 6);
    Assertions::assert_true(res_mid.is_some());
    Assertions::assert_equals(static_cast<usize>(2), res_mid.unwrap());
}

void test_twoway_find_utf8_bytes() {
    auto hay = reinterpret_cast<const u8*>("h\xF0\x9F\x98\x80world");
    auto pat = reinterpret_cast<const u8*>("\xF0\x9F\x98\x80");
    auto res = twoway_find(hay, 9, pat, 4);
    Assertions::assert_true(res.is_some());
    Assertions::assert_equals(static_cast<usize>(1), res.unwrap());
}

void test_twoway_find_match_at_end() {
    auto hay = reinterpret_cast<const u8*>("hello world");
    auto res = twoway_find(hay, 11, reinterpret_cast<const u8*>("world"), 5);
    Assertions::assert_true(res.is_some());
    Assertions::assert_equals(static_cast<usize>(6), res.unwrap());

    auto res_exact = twoway_find(hay, 11, hay, 11);
    Assertions::assert_true(res_exact.is_some());
    Assertions::assert_equals(static_cast<usize>(0), res_exact.unwrap());
}

void test_kmp_find_basic() {
    auto hay = reinterpret_cast<const u8*>("hello world");
    auto res = kmp_find(hay, 11, reinterpret_cast<const u8*>("world"), 5);
    Assertions::assert_true(res.is_some());
    Assertions::assert_equals(static_cast<usize>(6), res.unwrap());

    auto res_start = kmp_find(hay, 11, reinterpret_cast<const u8*>("hello"), 5);
    Assertions::assert_true(res_start.is_some());
    Assertions::assert_equals(static_cast<usize>(0), res_start.unwrap());
}

void test_kmp_find_no_match() {
    auto hay = reinterpret_cast<const u8*>("hello world");
    auto res = kmp_find(hay, 11, reinterpret_cast<const u8*>("xyz"), 3);
    Assertions::assert_true(res.is_none());
}

void test_kmp_find_empty() {
    auto hay = reinterpret_cast<const u8*>("hello");
    auto res_null = kmp_find(hay, 5, nullptr, 5);
    Assertions::assert_true(res_null.is_none());

    auto res_empty = kmp_find(hay, 5, reinterpret_cast<const u8*>(""), 0);
    Assertions::assert_true(res_empty.is_none());
}

void test_kmp_find_single_char() {
    auto hay = reinterpret_cast<const u8*>("hello");
    auto res = kmp_find(hay, 5, reinterpret_cast<const u8*>("l"), 1);
    Assertions::assert_true(res.is_some());
    Assertions::assert_equals(static_cast<usize>(2), res.unwrap());
}

void test_kmp_find_repeated_chars() {
    auto res = kmp_find(reinterpret_cast<const u8*>("aaaaa"), 5, reinterpret_cast<const u8*>("aaa"), 3);
    Assertions::assert_true(res.is_some());
    Assertions::assert_equals(static_cast<usize>(0), res.unwrap());
}

void test_kmp_find_all_no_match() {
    auto hay = reinterpret_cast<const u8*>("hello world");
    auto res = kmp_find_all(hay, 11, reinterpret_cast<const u8*>("xyz"), 3);
    Assertions::assert_equals(static_cast<usize>(0), res.len());
}

void test_kmp_find_all_basic() {
    auto hay = reinterpret_cast<const u8*>("abcabcabc");
    auto res = kmp_find_all(hay, 9, reinterpret_cast<const u8*>("abc"), 3);
    Assertions::assert_equals(static_cast<usize>(3), res.len());
    Assertions::assert_equals(static_cast<usize>(0), res.at(0));
    Assertions::assert_equals(static_cast<usize>(3), res.at(1));
    Assertions::assert_equals(static_cast<usize>(6), res.at(2));
}

void test_kmp_find_all_multiple() {
    auto hay = reinterpret_cast<const u8*>("abababab");
    auto res = kmp_find_all(hay, 8, reinterpret_cast<const u8*>("ab"), 2);
    Assertions::assert_equals(static_cast<usize>(4), res.len());
    Assertions::assert_equals(static_cast<usize>(0), res.at(0));
    Assertions::assert_equals(static_cast<usize>(2), res.at(1));
    Assertions::assert_equals(static_cast<usize>(4), res.at(2));
    Assertions::assert_equals(static_cast<usize>(6), res.at(3));
}

void test_kmp_find_all_overlapping() {
    auto res = kmp_find_all(reinterpret_cast<const u8*>("aaaaa"), 5, reinterpret_cast<const u8*>("aa"), 2);
    Assertions::assert_equals(static_cast<usize>(4), res.len());
    Assertions::assert_equals(static_cast<usize>(0), res.at(0));
    Assertions::assert_equals(static_cast<usize>(1), res.at(1));
    Assertions::assert_equals(static_cast<usize>(2), res.at(2));
    Assertions::assert_equals(static_cast<usize>(3), res.at(3));
}

void test_kmp_find_all_empty() {
    auto hay = reinterpret_cast<const u8*>("hello");
    auto res_null = kmp_find_all(hay, 5, nullptr, 5);
    Assertions::assert_equals(static_cast<usize>(0), res_null.len());

    auto res_empty = kmp_find_all(hay, 5, reinterpret_cast<const u8*>(""), 0);
    Assertions::assert_equals(static_cast<usize>(0), res_empty.len());
}

void test_kmp_find_all_single_char() {
    auto hay = reinterpret_cast<const u8*>("abcabc");
    auto res = kmp_find_all(hay, 6, reinterpret_cast<const u8*>("a"), 1);
    Assertions::assert_equals(static_cast<usize>(2), res.len());
    Assertions::assert_equals(static_cast<usize>(0), res.at(0));
    Assertions::assert_equals(static_cast<usize>(3), res.at(1));
}

GROUP_NAME("test_string_algorithm");
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(test_twoway_find_basic),
    UNIT_TEST_ITEM(test_twoway_find_no_match),
    UNIT_TEST_ITEM(test_twoway_find_empty_pattern),
    UNIT_TEST_ITEM(test_twoway_find_pattern_longer_than_text),
    UNIT_TEST_ITEM(test_twoway_find_single_char),
    UNIT_TEST_ITEM(test_twoway_find_repeated_chars),
    UNIT_TEST_ITEM(test_twoway_find_periodic_pattern),
    UNIT_TEST_ITEM(test_twoway_find_utf8_bytes),
    UNIT_TEST_ITEM(test_twoway_find_match_at_end),
    UNIT_TEST_ITEM(test_kmp_find_basic),
    UNIT_TEST_ITEM(test_kmp_find_no_match),
    UNIT_TEST_ITEM(test_kmp_find_empty),
    UNIT_TEST_ITEM(test_kmp_find_single_char),
    UNIT_TEST_ITEM(test_kmp_find_repeated_chars),
    UNIT_TEST_ITEM(test_kmp_find_all_no_match),
    UNIT_TEST_ITEM(test_kmp_find_all_basic),
    UNIT_TEST_ITEM(test_kmp_find_all_multiple),
    UNIT_TEST_ITEM(test_kmp_find_all_overlapping),
    UNIT_TEST_ITEM(test_kmp_find_all_empty),
    UNIT_TEST_ITEM(test_kmp_find_all_single_char));

} // namespace my::test::test_string_algorithm
