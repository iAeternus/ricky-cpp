/**
 * @brief UTF-8 编解码工具函数
 * @author Ricky
 * @date 2026/2/16
 * @version 1.0
 */
#ifndef STR_STRING_UTF8_HPP
#define STR_STRING_UTF8_HPP

#include "my_types.hpp"
#include "my_exception.hpp"

namespace my::str::detail {

/**
 * @brief 判断是否为合法的 Unicode 标量值
 * @param cp Unicode 码点
 * @return true=合法 false=非法
 * @note 合法范围：0x0000~0x10FFFF，排除 0xD800~0xDFFF（代理区）
 */
inline bool is_valid_scalar(const u32 cp) {
    return cp <= 0x10FFFFu && !(cp >= 0xD800u && cp <= 0xDFFFu);
}

/**
 * @brief 根据 UTF-8 首字节计算字符长度
 * @param lead 首字节
 * @return 字符长度（1~4），0 表示非法首字节
 */
inline usize utf8_char_len(const u8 lead) {
    if (lead < 0x80u) return 1;
    if ((lead >> 5) == 0x6) return 2;
    if ((lead >> 4) == 0xE) return 3;
    if ((lead >> 3) == 0x1E) return 4;
    return 0;
}

/**
 * @brief 从字节流中解码下一个 UTF-8 字符
 * @param p 输入输出参数，指向当前读取位置，解码后向后移动
 * @param end 字节流结束位置
 * @param out 输出参数，解码得到的码点
 * @return true=解码成功 false=解码失败（非法编码或截断）
 */
inline bool decode_next(const u8*& p, const u8* end, char32_t& out) {
    if (p >= end) return false;
    const u8 lead = *p;
    if (lead < 0x80u) {
        out = static_cast<char32_t>(lead);
        ++p;
        return true;
    }

    const usize len = utf8_char_len(lead);
    if (len == 0 || static_cast<usize>(end - p) < len) {
        return false;
    }

    u32 cp = 0;
    if (len == 2) {
        const u8 b1 = p[1];
        if ((b1 & 0xC0u) != 0x80u) return false;
        cp = (static_cast<u32>(lead & 0x1Fu) << 6) | (static_cast<u32>(b1 & 0x3Fu));
        if (cp < 0x80u) return false;
    } else if (len == 3) {
        const u8 b1 = p[1];
        const u8 b2 = p[2];
        if ((b1 & 0xC0u) != 0x80u || (b2 & 0xC0u) != 0x80u) return false;
        cp = (static_cast<u32>(lead & 0x0Fu) << 12) | (static_cast<u32>(b1 & 0x3Fu) << 6) | (static_cast<u32>(b2 & 0x3Fu));
        if (cp < 0x800u) return false;
    } else {
        const u8 b1 = p[1];
        const u8 b2 = p[2];
        const u8 b3 = p[3];
        if ((b1 & 0xC0u) != 0x80u || (b2 & 0xC0u) != 0x80u || (b3 & 0xC0u) != 0x80u) {
            return false;
        }
        cp = (static_cast<u32>(lead & 0x07u) << 18) | (static_cast<u32>(b1 & 0x3Fu) << 12) | (static_cast<u32>(b2 & 0x3Fu) << 6) | (static_cast<u32>(b3 & 0x3Fu));
        if (cp < 0x10000u) return false;
    }

    if (!is_valid_scalar(cp)) return false;
    out = static_cast<char32_t>(cp);
    p += len;
    return true;
}

/**
 * @brief 将 Unicode 码点编码为 UTF-8 字节序列
 * @param cp 要编码的码点
 * @param out 输出缓冲区，需至少 4 字节
 * @return 编码后的字节数（1~4）
 * @exception runtime_exception 若 cp 不是合法的 Unicode 标量值，则抛出运行时异常
 */
inline usize encode_utf8(const char32_t cp, u8* out) {
    const u32 v = static_cast<u32>(cp);
    if (!is_valid_scalar(v)) {
        throw runtime_exception("Invalid Unicode scalar value");
    }
    if (v < 0x80u) {
        out[0] = static_cast<u8>(v);
        return 1;
    }
    if (v < 0x800u) {
        out[0] = static_cast<u8>(0xC0u | (v >> 6));
        out[1] = static_cast<u8>(0x80u | (v & 0x3Fu));
        return 2;
    }
    if (v < 0x10000u) {
        out[0] = static_cast<u8>(0xE0u | (v >> 12));
        out[1] = static_cast<u8>(0x80u | ((v >> 6) & 0x3Fu));
        out[2] = static_cast<u8>(0x80u | (v & 0x3Fu));
        return 3;
    }
    out[0] = static_cast<u8>(0xF0u | (v >> 18));
    out[1] = static_cast<u8>(0x80u | ((v >> 12) & 0x3Fu));
    out[2] = static_cast<u8>(0x80u | ((v >> 6) & 0x3Fu));
    out[3] = static_cast<u8>(0x80u | (v & 0x3Fu));
    return 4;
}

} // namespace my::str::detail

#endif // STR_STRING_UTF8_HPP
