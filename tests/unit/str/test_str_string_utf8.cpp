#include "test_str_string_utf8.hpp"
#include "string.hpp"
#include "string_utf8.hpp"
#include "ricky_test.hpp"

namespace my::test::test_str_string_utf8 {

using namespace my::str;

namespace {

std::string to_std(StringView view) {
    return std::string(reinterpret_cast<const char*>(view.as_bytes()), view.len());
}

usize utf8_bytes_for_cp(char32_t cp) {
    u8 buf[4];
    return detail::encode_utf8(cp, buf);
}

} // namespace

void test_utf8_boundary_code_points() {
    // U+0000: 1-byte (null, single byte)
    {
        u8 buf[4];
        usize n = detail::encode_utf8(U'\0', buf);
        Assertions::assert_equals(static_cast<usize>(1), n);
        Assertions::assert_equals(static_cast<u8>(0x00), buf[0]);
        // decode back
        char32_t cp = 0;
        const u8* p = buf;
        bool ok = detail::decode_next(p, buf + n, cp);
        Assertions::assert_true(ok);
        Assertions::assert_equals(static_cast<char32_t>(0), cp);
    }

    // U+0080: 2-byte boundary (first 2-byte codepoint)
    {
        u8 buf[4];
        usize n = detail::encode_utf8(static_cast<char32_t>(0x80), buf);
        Assertions::assert_equals(static_cast<usize>(2), n);
        Assertions::assert_equals(static_cast<u8>(0xC2), buf[0]);
        Assertions::assert_equals(static_cast<u8>(0x80), buf[1]);
    }

    // U+0800: 3-byte boundary (first 3-byte codepoint)
    {
        u8 buf[4];
        usize n = detail::encode_utf8(static_cast<char32_t>(0x800), buf);
        Assertions::assert_equals(static_cast<usize>(3), n);
        Assertions::assert_equals(static_cast<u8>(0xE0), buf[0]);
        Assertions::assert_equals(static_cast<u8>(0xA0), buf[1]);
        Assertions::assert_equals(static_cast<u8>(0x80), buf[2]);
    }

    // U+10000: 4-byte boundary (first 4-byte codepoint)
    {
        u8 buf[4];
        usize n = detail::encode_utf8(static_cast<char32_t>(0x10000), buf);
        Assertions::assert_equals(static_cast<usize>(4), n);
        Assertions::assert_equals(static_cast<u8>(0xF0), buf[0]);
    }

    // U+D7FF: just below surrogate range (valid)
    {
        String s;
        s.push(static_cast<char32_t>(0xD7FF));
        Assertions::assert_equals(static_cast<usize>(3), s.len());
        auto cp = s.pop();
        Assertions::assert_true(cp.is_some());
        Assertions::assert_equals(static_cast<char32_t>(0xD7FF), cp.unwrap());
    }

    // U+E000: just above surrogate range (valid)
    {
        String s;
        s.push(static_cast<char32_t>(0xE000));
        Assertions::assert_equals(static_cast<usize>(3), s.len());
        auto cp = s.pop();
        Assertions::assert_true(cp.is_some());
        Assertions::assert_equals(static_cast<char32_t>(0xE000), cp.unwrap());
    }

    // U+10FFFF: max valid codepoint
    {
        u8 buf[4];
        usize n = detail::encode_utf8(static_cast<char32_t>(0x10FFFF), buf);
        Assertions::assert_equals(static_cast<usize>(4), n);
        char32_t cp = 0;
        const u8* p = buf;
        bool ok = detail::decode_next(p, buf + n, cp);
        Assertions::assert_true(ok);
        Assertions::assert_equals(static_cast<char32_t>(0x10FFFF), cp);
    }
}

void test_utf8_multilanguage_chinese() {
    String s("你好世界");
    Assertions::assert_equals(static_cast<usize>(12), s.len());
    Assertions::assert_equals(static_cast<usize>(4), s.chars().count());

    // 验证每个字
    auto chars = s.chars();
    auto c0 = chars.nth(0);
    auto c1 = chars.nth(1);
    auto c2 = chars.nth(2);
    auto c3 = chars.nth(3);
    Assertions::assert_true(c0.is_some());
    Assertions::assert_equals(static_cast<char32_t>(0x4F60), c0.unwrap()); // 你
    Assertions::assert_true(c1.is_some());
    Assertions::assert_equals(static_cast<char32_t>(0x597D), c1.unwrap()); // 好
    Assertions::assert_true(c2.is_some());
    Assertions::assert_equals(static_cast<char32_t>(0x4E16), c2.unwrap()); // 世
    Assertions::assert_true(c3.is_some());
    Assertions::assert_equals(static_cast<char32_t>(0x754C), c3.unwrap()); // 界
}

void test_utf8_multilanguage_japanese() {
    String s("こんにちは");
    Assertions::assert_equals(static_cast<usize>(15), s.len()); // 5 chars × 3 bytes
    Assertions::assert_equals(static_cast<usize>(5), s.chars().count());
    Assertions::assert_equals(std::string("こんにちは"), to_std(s.as_str()));
}

void test_utf8_multilanguage_korean() {
    String s("안녕하세요");
    Assertions::assert_equals(static_cast<usize>(15), s.len()); // 5 chars × 3 bytes
    Assertions::assert_equals(static_cast<usize>(5), s.chars().count());

    // push/pop roundtrip
    String s2;
    for (auto c : s.chars()) {
        s2.push(c);
    }
    Assertions::assert_equals(static_cast<usize>(15), s2.len());
    Assertions::assert_equals(to_std(s.as_str()), to_std(s2.as_str()));
}

void test_utf8_multilanguage_russian() {
    String s("Привет");
    Assertions::assert_equals(static_cast<usize>(12), s.len()); // 6 chars × 2 bytes
    Assertions::assert_equals(static_cast<usize>(6), s.chars().count());

    // 验证每个字母
    auto chars = s.chars();
    Assertions::assert_equals(static_cast<char32_t>(0x041F), chars.nth(0).unwrap()); // П
    Assertions::assert_equals(static_cast<char32_t>(0x0440), chars.nth(1).unwrap()); // р
    Assertions::assert_equals(static_cast<char32_t>(0x0438), chars.nth(2).unwrap()); // и
    Assertions::assert_equals(static_cast<char32_t>(0x0432), chars.nth(3).unwrap()); // в
    Assertions::assert_equals(static_cast<char32_t>(0x0435), chars.nth(4).unwrap()); // е
    Assertions::assert_equals(static_cast<char32_t>(0x0442), chars.nth(5).unwrap()); // т
}

void test_utf8_multilanguage_arabic() {
    String s("السلام");
    // Arabic letters are 2 bytes each in UTF-8
    Assertions::assert_equals(static_cast<usize>(12), s.len()); // 6 chars × 2 bytes
    Assertions::assert_equals(static_cast<usize>(6), s.chars().count());

    // Forward iteration works byte-level, but RTL rendering is UI concern
    auto last_ch = s.chars().last();
    Assertions::assert_true(last_ch.is_some());
    // م is U+0645
    Assertions::assert_equals(static_cast<char32_t>(0x0645), last_ch.unwrap());
}

void test_utf8_multilanguage_greek() {
    String s("Γειά σου");
    // Γ(2)ε(2)ι(2)ά(2) + ' '(1) + σ(2)ο(2)υ(2) = 15 bytes, 8 chars
    Assertions::assert_equals(static_cast<usize>(15), s.len());
    Assertions::assert_equals(static_cast<usize>(8), s.chars().count());
}

void test_utf8_emoji_basic() {
    // 😀 U+1F600
    String s("😀");
    Assertions::assert_equals(static_cast<usize>(4), s.len());
    Assertions::assert_equals(static_cast<usize>(1), s.chars().count());
    auto cp = s.chars().nth(0);
    Assertions::assert_true(cp.is_some());
    Assertions::assert_equals(static_cast<char32_t>(0x1F600), cp.unwrap());

    // Multiple emoji: 😀🎉🚀
    String s2("😀🎉🚀");
    Assertions::assert_equals(static_cast<usize>(12), s2.len()); // 3 × 4 bytes
    Assertions::assert_equals(static_cast<usize>(3), s2.chars().count());
    Assertions::assert_equals(static_cast<char32_t>(0x1F600), s2.chars().nth(0).unwrap());
    Assertions::assert_equals(static_cast<char32_t>(0x1F389), s2.chars().nth(1).unwrap()); // 🎉
    Assertions::assert_equals(static_cast<char32_t>(0x1F680), s2.chars().nth(2).unwrap()); // 🚀

    // Roundtrip via push
    String s3;
    for (auto c : s2.chars()) {
        s3.push(c);
    }
    Assertions::assert_equals(to_std(s2.as_str()), to_std(s3.as_str()));
}

void test_utf8_emoji_skin_tones() {
    // 👍 U+1F44D, 👍🏽 U+1F44D + U+1F3FD (skin tone)
    String s("👍");
    Assertions::assert_equals(static_cast<usize>(4), s.len());
    Assertions::assert_equals(static_cast<char32_t>(0x1F44D), s.chars().nth(0).unwrap());

    // 👍🏽 = thumbs up + skin tone (two code points)
    String s2("👍\U0001F3FD");
    Assertions::assert_equals(static_cast<usize>(8), s2.len()); // 4 + 4 bytes
    Assertions::assert_equals(static_cast<usize>(2), s2.chars().count());
    Assertions::assert_equals(static_cast<char32_t>(0x1F44D), s2.chars().nth(0).unwrap());
    Assertions::assert_equals(static_cast<char32_t>(0x1F3FD), s2.chars().nth(1).unwrap());
}

void test_utf8_emoji_zwj_sequences() {
    // 👨‍👩‍👧‍👦 U+1F468 + ZWJ + U+1F469 + ZWJ + U+1F467 + ZWJ + U+1F466
    // This is a family emoji constructed via ZWJ
    // For testing: just check that individual components are found
    const u8 zwj_seq[] = {
        0xF0, 0x9F, 0x91, 0xA8, // 👨 U+1F468
        0xE2, 0x80, 0x8D,       // ZWJ U+200D
        0xF0, 0x9F, 0x91, 0xA9, // 👩 U+1F469
        0xE2, 0x80, 0x8D,       // ZWJ
        0xF0, 0x9F, 0x91, 0xA7, // 👧 U+1F467
        0xE2, 0x80, 0x8D,       // ZWJ
        0xF0, 0x9F, 0x91, 0xA6  // 👦 U+1F466
    };
    StringView sv(zwj_seq, sizeof(zwj_seq));
    Assertions::assert_equals(static_cast<usize>(25), sv.len());
    Assertions::assert_equals(static_cast<usize>(7), sv.chars().count()); // 4 people + 3 ZWJ

    // Verify ZWJ is present
    Assertions::assert_equals(static_cast<char32_t>(0x200D), sv.chars().nth(1).unwrap());
    Assertions::assert_equals(static_cast<char32_t>(0x200D), sv.chars().nth(3).unwrap());
    Assertions::assert_equals(static_cast<char32_t>(0x200D), sv.chars().nth(5).unwrap());

    // Roundtrip through String
    String s;
    for (auto c : sv.chars()) {
        s.push(c);
    }
    Assertions::assert_equals(static_cast<usize>(25), s.len());
    Assertions::assert_equals(static_cast<usize>(7), s.chars().count());
}

void test_utf8_emoji_flags() {
    // 🇨🇳 China flag = U+1F1E8 U+1F1F3 (2 regional indicator symbols)
    const u8 cn_flag[] = {0xF0, 0x9F, 0x87, 0xA8, 0xF0, 0x9F, 0x87, 0xB3};
    StringView sv(cn_flag, 8);
    Assertions::assert_equals(static_cast<usize>(8), sv.len());
    Assertions::assert_equals(static_cast<usize>(2), sv.chars().count());
    Assertions::assert_equals(static_cast<char32_t>(0x1F1E8), sv.chars().nth(0).unwrap());
    Assertions::assert_equals(static_cast<char32_t>(0x1F1F3), sv.chars().nth(1).unwrap());

    // 🇯🇵 Japan flag = U+1F1EF U+1F1F5
    const u8 jp_flag[] = {0xF0, 0x9F, 0x87, 0xAF, 0xF0, 0x9F, 0x87, 0xB5};
    StringView sv2(jp_flag, 8);
    Assertions::assert_equals(static_cast<usize>(2), sv2.chars().count());

    // Roundtrip
    String s;
    s.push(sv.chars().nth(0).unwrap());
    s.push(sv.chars().nth(1).unwrap());
    Assertions::assert_equals(static_cast<usize>(8), s.len());
}

void test_utf8_combining_characters() {
    // e + combining acute accent = é (canonically U+00E9)
    String s("e\u0301");                                                 // e + combining acute accent
    Assertions::assert_equals(static_cast<usize>(3), s.len());           // 1 byte + 2 bytes
    Assertions::assert_equals(static_cast<usize>(2), s.chars().count()); // 2 code points
    Assertions::assert_equals(static_cast<char32_t>(0x0065), s.chars().nth(0).unwrap());
    Assertions::assert_equals(static_cast<char32_t>(0x0301), s.chars().nth(1).unwrap()); // combining accent

    // n + combining tilde = ñ
    String s2("n\u0303");
    Assertions::assert_equals(static_cast<usize>(3), s2.len());
    Assertions::assert_equals(static_cast<usize>(2), s2.chars().count());

    // a + combining diaeresis = ä
    String s3("a\u0308");
    Assertions::assert_equals(static_cast<usize>(3), s3.len());
    Assertions::assert_equals(static_cast<usize>(2), s3.chars().count());
}

void test_utf8_encode_decode_roundtrip() {
    // 测试 1-4 字节各个边界值
    const char32_t test_cps[] = {
        0x0000, 0x0001, 0x007F,    // 1-byte range
        0x0080, 0x00FF, 0x07FF,    // 2-byte range
        0x0800, 0x0FFF, 0xFFFF,    // 3-byte range (excluding surrogate)
        0x10000, 0x1F600, 0x10FFFF // 4-byte range
    };

    for (auto expected_cp : test_cps) {
        if ((expected_cp >= 0xD800u && expected_cp <= 0xDFFFu)) continue;

        u8 buf[4];
        usize n = detail::encode_utf8(expected_cp, buf);

        char32_t decoded_cp = 0;
        const u8* p = buf;
        bool ok = detail::decode_next(p, buf + n, decoded_cp);

        Assertions::assert_true(ok);
        Assertions::assert_equals(expected_cp, decoded_cp);
    }
}

void test_utf8_push_all_ranges() {
    String s;

    // 1-byte (ASCII)
    s.push(static_cast<char32_t>(0x0041)); // 'A'
    // 2-byte
    s.push(static_cast<char32_t>(0x00DF)); // ß
    // 3-byte
    s.push(static_cast<char32_t>(0x4E2D)); // 中
    // 4-byte
    s.push(static_cast<char32_t>(0x1F600)); // 😀

    Assertions::assert_equals(static_cast<usize>(1 + 2 + 3 + 4), s.len()); // 10 bytes
    Assertions::assert_equals(static_cast<usize>(4), s.chars().count());
    Assertions::assert_equals(static_cast<char32_t>(0x0041), s.chars().nth(0).unwrap());
    Assertions::assert_equals(static_cast<char32_t>(0x00DF), s.chars().nth(1).unwrap());
    Assertions::assert_equals(static_cast<char32_t>(0x4E2D), s.chars().nth(2).unwrap());
    Assertions::assert_equals(static_cast<char32_t>(0x1F600), s.chars().nth(3).unwrap());
}

void test_utf8_pop_all_ranges() {
    String s;
    s.push(static_cast<char32_t>(0x0041));  // 'A' 1 byte
    s.push(static_cast<char32_t>(0x00DF));  // ß  2 bytes
    s.push(static_cast<char32_t>(0x4E2D));  // 中 3 bytes
    s.push(static_cast<char32_t>(0x1F600)); // 😀 4 bytes
    Assertions::assert_equals(static_cast<usize>(10), s.len());

    // Pop 4-byte
    auto cp3 = s.pop();
    Assertions::assert_true(cp3.is_some());
    Assertions::assert_equals(static_cast<char32_t>(0x1F600), cp3.unwrap());
    Assertions::assert_equals(static_cast<usize>(6), s.len());

    // Pop 3-byte
    auto cp2 = s.pop();
    Assertions::assert_true(cp2.is_some());
    Assertions::assert_equals(static_cast<char32_t>(0x4E2D), cp2.unwrap());
    Assertions::assert_equals(static_cast<usize>(3), s.len());

    // Pop 2-byte
    auto cp1 = s.pop();
    Assertions::assert_true(cp1.is_some());
    Assertions::assert_equals(static_cast<char32_t>(0x00DF), cp1.unwrap());
    Assertions::assert_equals(static_cast<usize>(1), s.len());

    // Pop 1-byte
    auto cp0 = s.pop();
    Assertions::assert_true(cp0.is_some());
    Assertions::assert_equals(static_cast<char32_t>(0x0041), cp0.unwrap());
    Assertions::assert_true(s.is_empty());

    // Pop from empty string
    auto none = s.pop();
    Assertions::assert_true(none.is_none());
}

void test_utf8_chars_iteration() {
    // 混合 1/2/3/4 字节
    String s("Aß中😀");
    // A(1) ß(2) 中(3) 😀(4) = 10 bytes
    Assertions::assert_equals(static_cast<usize>(10), s.len());
    Assertions::assert_equals(static_cast<usize>(4), s.chars().count());

    // enumerate iteration
    usize idx_sum = 0;
    usize cp_sum = 0;
    for (auto [idx, cp] : s.chars().enumerate()) {
        idx_sum += idx;
        cp_sum += static_cast<usize>(cp);
    }
    Assertions::assert_equals(static_cast<usize>(0 + 1 + 2 + 3), idx_sum);
    Assertions::assert_equals(static_cast<usize>(0x0041 + 0x00DF + 0x4E2D + 0x1F600), cp_sum);

    // bytes() enumerate
    usize byte_idx_sum = 0;
    for (auto [idx, b] : s.bytes().enumerate()) {
        (void)b;
        byte_idx_sum += idx;
    }
    Assertions::assert_equals(static_cast<usize>(45), byte_idx_sum); // 0+1+...+9 = 45

    // nth() various positions
    auto chars = s.chars();
    Assertions::assert_equals(static_cast<char32_t>(0x0041), chars.nth(0).unwrap());
    Assertions::assert_equals(static_cast<char32_t>(0x00DF), chars.nth(1).unwrap());
    Assertions::assert_equals(static_cast<char32_t>(0x4E2D), chars.nth(2).unwrap());
    Assertions::assert_equals(static_cast<char32_t>(0x1F600), chars.nth(3).unwrap());
    Assertions::assert_true(chars.nth(4).is_none());

    // last()
    Assertions::assert_equals(static_cast<char32_t>(0x1F600), chars.last().unwrap());

    // empty string chars
    String empty;
    Assertions::assert_equals(static_cast<usize>(0), empty.chars().count());
    Assertions::assert_true(empty.chars().nth(0).is_none());
    Assertions::assert_true(empty.chars().last().is_none());
}

void test_utf8_validation_reject_overlong() {
    // 2-byte overlong: 0xC0 0x80 should decode to U+0000 (rejected)
    const u8 overlong_2[] = {0xC0, 0x80};
    StringView sv(overlong_2, 2);
    Assertions::assert_throws("Invalid UTF-8", [&]() {
        volatile auto cp = sv.chars().nth(0);
        (void)cp;
    });

    // 3-byte overlong: 0xE0 0x80 0x80 should decode to U+0000 (rejected)
    const u8 overlong_3[] = {0xE0, 0x80, 0x80};
    StringView sv2(overlong_3, 3);
    Assertions::assert_throws("Invalid UTF-8", [&]() {
        volatile auto cp = sv2.chars().nth(0);
        (void)cp;
    });

    // 3-byte overlong encoding of U+007F: 0xE0 0x81 0xBF (rejected because < 0x800)
    const u8 overlong_3b[] = {0xE0, 0x81, 0xBF};
    StringView sv3(overlong_3b, 3);
    Assertions::assert_throws("Invalid UTF-8", [&]() {
        volatile auto cp = sv3.chars().nth(0);
        (void)cp;
    });

    // 4-byte overlong: 0xF0 0x80 0x80 0x80 (rejected)
    const u8 overlong_4[] = {0xF0, 0x80, 0x80, 0x80};
    StringView sv4(overlong_4, 4);
    Assertions::assert_throws("Invalid UTF-8", [&]() {
        volatile auto cp = sv4.chars().nth(0);
        (void)cp;
    });
}

void test_utf8_validation_reject_surrogate() {
    // U+D800 encoded in UTF-8: 0xED 0xA0 0x80
    const u8 surrogate[] = {0xED, 0xA0, 0x80};
    StringView sv(surrogate, 3);
    Assertions::assert_throws("Invalid UTF-8", [&]() {
        volatile auto cp = sv.chars().nth(0);
        (void)cp;
    });

    // U+DFFF encoded in UTF-8: 0xED 0xBF 0xBF
    const u8 surrogate_end[] = {0xED, 0xBF, 0xBF};
    StringView sv2(surrogate_end, 3);
    Assertions::assert_throws("Invalid UTF-8", [&]() {
        volatile auto cp = sv2.chars().nth(0);
        (void)cp;
    });

    // Also test via String::push (should throw on encode)
    String s;
    Assertions::assert_throws("Invalid Unicode scalar value", [&]() {
        s.push(static_cast<char32_t>(0xD800));
    });
    Assertions::assert_throws("Invalid Unicode scalar value", [&]() {
        s.push(static_cast<char32_t>(0xDFFF));
    });
}

void test_utf8_validation_reject_truncated() {
    // Missing continuation byte
    const u8 truncated[] = {0xE0, 0xA0}; // starts 3-byte but only 2 bytes
    StringView sv(truncated, 2);
    Assertions::assert_throws("Invalid UTF-8", [&]() {
        volatile auto cp = sv.chars().nth(0);
        (void)cp;
    });

    // Invalid continuation byte (0x00 instead of 0x80~0xBF)
    const u8 bad_cont[] = {0xE0, 0xA0, 0x00};
    StringView sv2(bad_cont, 3);
    Assertions::assert_throws("Invalid UTF-8", [&]() {
        volatile auto cp = sv2.chars().nth(0);
        (void)cp;
    });

    // 4-byte truncated to 3 bytes
    const u8 truncated_4[] = {0xF0, 0x9F, 0x98};
    StringView sv3(truncated_4, 3);
    Assertions::assert_throws("Invalid UTF-8", [&]() {
        volatile auto cp = sv3.chars().nth(0);
        (void)cp;
    });

    // Invalid lead byte (0xFF)
    const u8 bad_lead[] = {0xFF};
    StringView sv4(bad_lead, 1);
    Assertions::assert_throws("Invalid UTF-8", [&]() {
        volatile auto cp = sv4.chars().nth(0);
        (void)cp;
    });

    // 0xFE is also invalid lead byte
    const u8 bad_lead2[] = {0xFE};
    StringView sv5(bad_lead2, 1);
    Assertions::assert_throws("Invalid UTF-8", [&]() {
        volatile auto cp = sv5.chars().nth(0);
        (void)cp;
    });
}

void test_utf8_validation_reject_too_large() {
    // U+110000 (beyond U+10FFFF): encoded as F4 90 80 80
    const u8 too_large[] = {0xF4, 0x90, 0x80, 0x80};
    StringView sv(too_large, 4);
    Assertions::assert_throws("Invalid UTF-8", [&]() {
        volatile auto cp = sv.chars().nth(0);
        (void)cp;
    });

    // U+1FFFFF (way too large): FB BF BF BF
    const u8 way_too_large[] = {0xFB, 0xBF, 0xBF, 0xBF};
    StringView sv2(way_too_large, 4);
    Assertions::assert_throws("Invalid UTF-8", [&]() {
        volatile auto cp = sv2.chars().nth(0);
        (void)cp;
    });
}

void test_utf8_case_conversion_non_ascii() {
    // 非 ASCII 大小写不变
    String s("ÄÖÜ");
    auto lower = s.to_lowercase();
    auto upper = s.to_uppercase();
    // 当前实现：非 ASCII case 不变
    Assertions::assert_equals(std::string("ÄÖÜ"), to_std(lower.as_str()));
    Assertions::assert_equals(std::string("ÄÖÜ"), to_std(upper.as_str()));

    // 混合 ASCII + 非 ASCII
    String s2("HelloÄÖÜWorld");
    auto lower2 = s2.to_lowercase();
    auto upper2 = s2.to_uppercase();
    Assertions::assert_equals(std::string("helloÄÖÜworld"), to_std(lower2.as_str()));
    Assertions::assert_equals(std::string("HELLOÄÖÜWORLD"), to_std(upper2.as_str()));
}

void test_utf8_push_emoji_and_find() {
    String s;
    s.push(static_cast<char32_t>(0x1F600));                    // 😀
    s.push(static_cast<char32_t>(0x1F44D));                    // 👍
    Assertions::assert_equals(static_cast<usize>(8), s.len()); // 4 + 4 bytes
    Assertions::assert_equals(static_cast<usize>(2), s.chars().count());

    // find emoji substring
    auto pos = s.find(StringView("\xF0\x9F\x98\x80", 4)); // 😀
    Assertions::assert_true(pos.is_some());
    Assertions::assert_equals(static_cast<usize>(0), pos.unwrap());

    auto pos2 = s.find(StringView("\xF0\x9F\x91\x8D", 4)); // 👍
    Assertions::assert_true(pos2.is_some());
    Assertions::assert_equals(static_cast<usize>(4), pos2.unwrap());
}

void test_utf8_string_view_utf8_roundtrip() {
    // StringView → String → StringView roundtrip preserves bytes
    const u8 multi_bytes[] = {
        'H', 'i',                           // ASCII
        0xE4, 0xBD, 0xA0, 0xE5, 0xA5, 0xBD, // 你好 (3+3)
        0xF0, 0x9F, 0x98, 0x80              // 😀 (4)
    };
    StringView original(multi_bytes, sizeof(multi_bytes));
    Assertions::assert_equals(static_cast<usize>(12), original.len());
    Assertions::assert_equals(static_cast<usize>(5), original.chars().count());

    // to_string() and back
    String s = original.to_string();
    Assertions::assert_equals(static_cast<usize>(12), s.len());

    StringView roundtrip = s.as_str();
    Assertions::assert_equals(static_cast<usize>(12), roundtrip.len());
    Assertions::assert_equals(static_cast<usize>(5), roundtrip.chars().count());

    // Verify each code point survives roundtrip
    for (usize i = 0; i < 5; ++i) {
        auto orig_cp = original.chars().nth(i);
        auto rt_cp = roundtrip.chars().nth(i);
        Assertions::assert_true(orig_cp.is_some());
        Assertions::assert_true(rt_cp.is_some());
        Assertions::assert_equals(orig_cp.unwrap(), rt_cp.unwrap());
    }
}

GROUP_NAME("test_str_string_utf8");
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(test_utf8_boundary_code_points),
    UNIT_TEST_ITEM(test_utf8_multilanguage_chinese),
    UNIT_TEST_ITEM(test_utf8_multilanguage_japanese),
    UNIT_TEST_ITEM(test_utf8_multilanguage_korean),
    UNIT_TEST_ITEM(test_utf8_multilanguage_russian),
    UNIT_TEST_ITEM(test_utf8_multilanguage_arabic),
    UNIT_TEST_ITEM(test_utf8_multilanguage_greek),
    UNIT_TEST_ITEM(test_utf8_emoji_basic),
    UNIT_TEST_ITEM(test_utf8_emoji_skin_tones),
    UNIT_TEST_ITEM(test_utf8_emoji_zwj_sequences),
    UNIT_TEST_ITEM(test_utf8_emoji_flags),
    UNIT_TEST_ITEM(test_utf8_combining_characters),
    UNIT_TEST_ITEM(test_utf8_encode_decode_roundtrip),
    UNIT_TEST_ITEM(test_utf8_push_all_ranges),
    UNIT_TEST_ITEM(test_utf8_pop_all_ranges),
    UNIT_TEST_ITEM(test_utf8_chars_iteration),
    UNIT_TEST_ITEM(test_utf8_validation_reject_overlong),
    UNIT_TEST_ITEM(test_utf8_validation_reject_surrogate),
    UNIT_TEST_ITEM(test_utf8_validation_reject_truncated),
    UNIT_TEST_ITEM(test_utf8_validation_reject_too_large),
    UNIT_TEST_ITEM(test_utf8_case_conversion_non_ascii),
    UNIT_TEST_ITEM(test_utf8_push_emoji_and_find),
    UNIT_TEST_ITEM(test_utf8_string_view_utf8_roundtrip));

} // namespace my::test::test_str_string_utf8
