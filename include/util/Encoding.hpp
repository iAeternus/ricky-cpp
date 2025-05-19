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

static const CString ASCII = "ascii";
static const CString UTF8 = "utf-8";
static const CString UTF16 = "utf-16";
static const CString UTF32 = "utf-32";
static const CString GB2312 = "gb2312";

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
        return "Encoding";
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
        return ASCII;
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
            ValueError("Invalid CodePoint");
        }
        return 0;
    }

    Self* clone() const override {
        return new Self{};
    }

    [[nodiscard]] CString __str__() const override {
        return UTF8;
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
        return UTF16;
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
        return UTF32;
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
        return GB2312;
    }
};

/**
 * @brief 编码名-编码方式 映射
 */
fn encoding_map(const CString& encodingName)->Encoding* {
    static Dict<CString, Encoding*> encodingMap_ = {
        {ASCII, new ASCIIEncoding{}},
        {UTF8, new UTF8Encoding{}},
        {UTF16, new UTF16Encoding{}},
        {UTF32, new UTF32Encoding{}},
        {GB2312, new GB2312Encoding{}},
    };

    return encodingMap_.get(encodingName);
}

} // namespace my::util

#endif // ENCODING_HPP