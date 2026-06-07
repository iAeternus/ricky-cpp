#ifndef TEST_STR_STRING_UTF8_HPP
#define TEST_STR_STRING_UTF8_HPP

namespace my::test::test_str_string_utf8 {

// 边界码点
void test_utf8_boundary_code_points();

// 多语言
void test_utf8_multilanguage_chinese();
void test_utf8_multilanguage_japanese();
void test_utf8_multilanguage_korean();
void test_utf8_multilanguage_russian();
void test_utf8_multilanguage_arabic();
void test_utf8_multilanguage_greek();

// Emoji
void test_utf8_emoji_basic();
void test_utf8_emoji_skin_tones();
void test_utf8_emoji_zwj_sequences();
void test_utf8_emoji_flags();

// 组合字符
void test_utf8_combining_characters();

// 编解码往返
void test_utf8_encode_decode_roundtrip();
void test_utf8_push_all_ranges();
void test_utf8_pop_all_ranges();

// chars 迭代
void test_utf8_chars_iteration();

// 非法 UTF-8 拒绝
void test_utf8_validation_reject_overlong();
void test_utf8_validation_reject_surrogate();
void test_utf8_validation_reject_truncated();
void test_utf8_validation_reject_too_large();

// 大小写转换 (仅 ASCII)
void test_utf8_case_conversion_non_ascii();

// Emoji push + find
void test_utf8_push_emoji_and_find();

// StringView UTF-8 往返
void test_utf8_string_view_utf8_roundtrip();

} // namespace my::test::test_str_string_utf8

#endif // TEST_STR_STRING_UTF8_HPP
