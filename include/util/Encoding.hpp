/**
 * @brief 字符编码
 * @author Ricky
 * @date 2024/12/19
 * @version 1.0
 */
#ifndef ENCODING_HPP
#define ENCODING_HPP

#include "Dict.hpp"

namespace my::util {

/**
 * @brief 字符编码类型
 */
enum class EncodingType {
    ASCII,
    UTF8,
    UTF16,
    UTF32,
    GB2312,
    LATIN1,
};

/**
 * @brief 字符编码接口
 */
class Encoding : Object<Encoding> {
public:
    using Self = Encoding;
    using Super = Object<Self>;

    virtual ~Encoding() = default;

    /**
     * @brief 返回当前编码方式，开头字符的字节数
     */
    virtual u8 byte_size(const char* data) const = 0;

    /**
     * @brief 返回当前编码方式的克隆
     */
    virtual Encoding* clone() const = 0;

    [[nodiscard]] virtual CString __str__() const {
        return "UNKNOWN";
    }
};

/**
 * @brief ASCII
 */
class ASCIIEncoding : public Encoding {
public:
    using Self = ASCIIEncoding;
    using Super = Encoding;

    u8 byte_size(const char* data) const override {
        (void)data; // cheer the compiler
        return 1;
    }

    Self* clone() const override {
        return new Self{};
    }

    [[nodiscard]] CString __str__() const override {
        return "ASCII";
    }
};

/**
 * @brief utf-8
 */
class UTF8Encoding : public Encoding {
public:
    using Self = UTF8Encoding;
    using Super = Encoding;

    u8 byte_size(const char* data) const override {
        if ((data[0] & 0x80) == 0) {
            return 1; // 以0    开头（0xxxxxxx），1字节编码
        } else if ((data[0] & 0xE0) == 0xC0) {
            return 2; // 以110  开头（110xxxxx），2字节编码
        } else if ((data[0] & 0xF0) == 0xE0) {
            return 3; // 以1110 开头（1110xxxx），3字节编码
        } else if ((data[0] & 0xF8) == 0xF0) {
            return 4; // 以11110开头（11110xxx），4字节编码
        } else {
            throw runtime_exception("invalid CodePoint");
        }
        return 0;
    }

    Self* clone() const override {
        return new Self{};
    }

    [[nodiscard]] CString __str__() const override {
        return "UTF-8";
    }
};

/**
 * @brief utf-16
 */
class UTF16Encoding : public Encoding {
public:
    using Self = UTF16Encoding;
    using Super = Encoding;

    u8 byte_size(const char* data) const override {
        if ((data[0] & 0xFC) == 0xD8) {
            return 4; // 以110110开头（110110xx 110111xx），4字节编码
        } else {
            return 2;
        }
    }

    Self* clone() const override {
        return new Self{};
    }

    [[nodiscard]] CString __str__() const override {
        return "UTF-16";
    }
};

/**
 * @brief utf-32
 */
class UTF32Encoding : public Encoding {
public:
    using Self = UTF32Encoding;
    using Super = Encoding;

    u8 byte_size(const char* data) const override {
        (void)data;
        return 4;
    }

    Self* clone() const override {
        return new Self{};
    }

    [[nodiscard]] CString __str__() const override {
        return "UTF-32";
    }
};

/**
 * @brief gb2312，暂未实现
 */
class GB2312Encoding : public Encoding {
public:
    using Self = GB2312Encoding;
    using Super = Encoding;

    u8 byte_size(const char* data) const override {
        (void)data;
        return 0;
    }

    Self* clone() const override {
        return new Self{};
    }

    [[nodiscard]] CString __str__() const override {
        return "GB2312";
    }
};

/**
 * @brief 编码类型-编码方式 映射
 */
fn encoding_map(EncodingType enc)->Encoding* {
    static const Dict<EncodingType, Encoding*> encoding_map_ = {
        {EncodingType::ASCII, new ASCIIEncoding{}},
        {EncodingType::UTF8, new UTF8Encoding{}},
        {EncodingType::UTF16, new UTF16Encoding{}},
        {EncodingType::UTF32, new UTF32Encoding{}},
        {EncodingType::GB2312, new GB2312Encoding{}},
    };

    return encoding_map_.get(enc);
}

} // namespace my::util

namespace std {

template <>
struct formatter<my::util::EncodingType> : formatter<string_view> {
    auto format(my::util::EncodingType type, format_context& ctx) const {
        using my::util::EncodingType;
        string_view name;
        switch (type) {
        case EncodingType::ASCII: name = "ASCII"; break;
        case EncodingType::UTF8: name = "UTF-8"; break;
        case EncodingType::UTF16: name = "UTF-16"; break;
        case EncodingType::UTF32: name = "UTF-32"; break;
        case EncodingType::GB2312: name = "GB2312"; break;
        case EncodingType::LATIN1: name = "LATIN1"; break;
        default: name = "UNKNOWN";
        }
        return formatter<string_view>::format(name, ctx);
    }
};

} // namespace std

#endif // ENCODING_HPP