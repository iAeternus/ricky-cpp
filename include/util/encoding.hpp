/**
 * @brief 字符编码
 * @author Ricky
 * @date 2024/12/19
 * @version 1.0
 */
#ifndef ENCODING_HPP
#define ENCODING_HPP

#include "my_exception.hpp"

namespace my::util {

/**
 * @brief 字符编码类型
 */
enum class EncodingType {
    UTF8,
    UTF16,
    UTF32,
    GB2312,
    LATIN1,
    ASCII,
};

/**
 * @brief 编码类型萃取
 */
template <EncodingType Enc>
struct EncodingTraits;

template <>
struct EncodingTraits<EncodingType::UTF8> {
    using Self = EncodingTraits<EncodingType::UTF8>;
    static constexpr usize MAX_CHAR_SIZE = 4;
    static constexpr auto NAME = "UTF-8";

    /**
     * @exception Exception 若码点不符合UTF-8格式，则抛出 runtime_exception
     */
    static u8 char_size(const char* data) {
        if ((data[0] & 0x80) == 0) {
            return 1; // 以0    开头（0xxxxxxx），1字节编码
        }
        if ((data[0] & 0xE0) == 0xC0) {
            return 2; // 以110  开头（110xxxxx），2字节编码
        }
        if ((data[0] & 0xF0) == 0xE0) {
            return 3; // 以1110 开头（1110xxxx），3字节编码
        }
        if ((data[0] & 0xF8) == 0xF0) {
            return 4; // 以11110开头（11110xxx），4字节编码
        }
        throw runtime_exception("Invalid CodePoint");
    }

    static bool is_valid(const char* data, const usize length) {
        if (length == 0) return false;

        const u8 size = char_size(data);
        if (length < size) return false;

        // 检查后续字节格式(10xxxxxx)
        for (u8 i = 1; i < size; ++i) {
            if ((data[i] & 0xC0) != 0x80) {
                return false;
            }
        }

        // 检查编码范围有效性
        if (size == 2 && (data[0] & 0x1E) == 0) return false; // 过短编码
        if (size == 3 && (data[0] & 0x0F) == 0 && (data[1] & 0x20) == 0) return false;
        if (size == 4 && (data[0] & 0x07) == 0 && (data[1] & 0x30) == 0) return false;

        return true;
    }
};

template <>
struct EncodingTraits<EncodingType::UTF16> {
    using Self = EncodingTraits<EncodingType::UTF16>;
    static constexpr usize MAX_CHAR_SIZE = 4;
    static constexpr auto NAME = "UTF-16";

    static u8 char_size(const char* data) {
        const auto* ptr = reinterpret_cast<const uint16_t*>(data);
        if (*ptr >= 0xD800 && *ptr <= 0xDBFF) {
            return 4; // 代理对
        } else {
            return 2;
        }
    }

    static bool is_valid(const char* data, const usize length) {
        if (length < 2) return false;

        const auto* ptr = reinterpret_cast<const uint16_t*>(data);
        const u16 first = *ptr;

        // 检查是否是高代理
        if (first >= 0xD800 && first <= 0xDBFF) {
            if (length < 4) return false;

            const auto* next = reinterpret_cast<const uint16_t*>(data + 2);
            const u16 second = *next;

            // 检查是否是低代理
            return (second >= 0xDC00 && second <= 0xDFFF);
        } else if (first >= 0xDC00 && first <= 0xDFFF) { // 检查是否是低代理(不能单独出现)
            return false;
        }

        return true;
    }
};

template <>
struct EncodingTraits<EncodingType::UTF32> {
    using Self = EncodingTraits<EncodingType::UTF32>;
    static constexpr usize MAX_CHAR_SIZE = 4;
    static constexpr auto NAME = "UTF-32";

    static u8 char_size(const char* data) {
        (void)data;
        return 4;
    }

    static bool is_valid(const char* data, const usize length) {
        if (length < 4) return false;

        const auto code_point = *reinterpret_cast<const u32*>(data);
        // UTF-32编码必须满足: 0 <= code_point <= 0x10FFFF
        // 且不能是代理码点(0xD800-0xDFFF)
        return code_point <= 0x10FFFF && (code_point < 0xD800 || code_point > 0xDFFF);
    }
};

template <>
struct EncodingTraits<EncodingType::GB2312> {
    using Self = EncodingTraits<EncodingType::GB2312>;
    static constexpr usize MAX_CHAR_SIZE = 2;
    static constexpr auto NAME = "GB2312";

    /**
     * @exception Exception 若码点不符合GB2312格式，则抛出 runtime_exception
     */
    static u8 char_size(const char* data) {
        const u8 first = static_cast<u8>(data[0]);
        if (first <= 0x7F) {
            return 1; // ASCII字符
        } else if (first >= 0xA1 && first <= 0xF7) {
            return 2; // GB2312汉字
        }
        throw runtime_exception("Invalid GB2312 sequence");
    }

    static bool is_valid(const char* data, const usize length) {
        if (length == 0) return false;

        const u8 first = static_cast<u8>(data[0]);
        if (first <= 0x7F) {
            return true; // ASCII字符总是有效
        } else if (first >= 0xA1 && first <= 0xF7) {
            if (length < 2) return false;
            const u8 second = static_cast<u8>(data[1]);
            return (second >= 0xA1 && second <= 0xFE);
        }

        return false;
    }
};

template <>
struct EncodingTraits<EncodingType::LATIN1> {
    using Self = EncodingTraits<EncodingType::LATIN1>;
    static constexpr usize MAX_CHAR_SIZE = 1;
    static constexpr auto NAME = "LATIN1";

    static u8 char_size(const char* data) {
        (void)data;
        return 1;
    }

    static bool is_valid(const char* data, const usize length) {
        (void)data;
        return length >= 1;
    }
};

template <>
struct EncodingTraits<EncodingType::ASCII> {
    using Self = EncodingTraits<EncodingType::ASCII>;
    static constexpr usize MAX_CHAR_SIZE = 1;
    static constexpr auto NAME = "ASCII";

    static u8 char_size(const char* data) {
        (void)data;
        return 1;
    }

    static bool is_valid(const char* data, const usize length) {
        return length >= 1 && (static_cast<u8>(data[0]) <= 0x7F);
    }
};

} // namespace my::util

template <>
struct std::formatter<my::util::EncodingType> : formatter<string_view> {
    auto format(my::util::EncodingType type, format_context& ctx) const {
        using my::util::EncodingType;
        std::string_view name;
        switch (type) {
        case EncodingType::ASCII: name = my::util::EncodingTraits<EncodingType::ASCII>::NAME; break;
        case EncodingType::UTF8: name = my::util::EncodingTraits<EncodingType::UTF8>::NAME; break;
        case EncodingType::UTF16: name = my::util::EncodingTraits<EncodingType::UTF16>::NAME; break;
        case EncodingType::UTF32: name = my::util::EncodingTraits<EncodingType::UTF32>::NAME; break;
        case EncodingType::GB2312: name = my::util::EncodingTraits<EncodingType::GB2312>::NAME; break;
        case EncodingType::LATIN1: name = my::util::EncodingTraits<EncodingType::LATIN1>::NAME; break;
        default: name = "UNKNOWN";
        }
        return formatter<string_view>::format(name, ctx);
    }
};

#endif // ENCODING_HPP
