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
    using self = Encoding;
    using super = Object<self>;

public:
    virtual ~Encoding() = default;

    /**
     * @brief 返回当前编码方式，开头字符的字节数
     */
    virtual int byte_size(const char* data) const = 0;

    /**
     * @brief 返回当前编码方式的克隆
     */
    virtual Encoding* clone() const = 0;

    virtual CString __str__() const {
        return "Encoding";
    }
};

/**
 * @brief ASCII
 */
class ASCIIEncoding : public Encoding {
    using self = ASCIIEncoding;
    using super = Encoding;

public:
    int byte_size(const char* data) const override {
        (void)data; // 告诉编译器可能不使用data
        return 1;
    }

    self* clone() const override {
        return new self{};
    }

    CString __str__() const override {
        return ASCII;
    }
};

/**
 * @brief utf-8
 */
class UTF8Encoding : public Encoding {
    using self = UTF8Encoding;
    using super = Encoding;

public:
    int byte_size(const char* data) const override {
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

    self* clone() const override {
        return new self{};
    }

    CString __str__() const override {
        return UTF8;
    }
};

/**
 * @brief utf-16
 */
class UTF16Encoding : public Encoding {
    using self = UTF16Encoding;
    using super = Encoding;

public:
    int byte_size(const char* data) const override {
        if ((data[0] & 0xFC) == 0xD8) {
            return 4; // 以110110开头（110110xx 110111xx），4字节编码
        } else {
            return 2;
        }
    }

    self* clone() const override {
        return new self{};
    }

    CString __str__() const override {
        return UTF16;
    }
};

/**
 * @brief utf-32
 */
class UTF32Encoding : public Encoding {
    using self = UTF32Encoding;
    using super = Encoding;

public:
    int byte_size(const char* data) const override {
        (void)data; // 告诉编译器可能不使用data
        return 4;
    }

    self* clone() const override {
        return new self{};
    }

    CString __str__() const override {
        return UTF32;
    }
};

/**
 * @brief gb2312，暂未实现
 */
class GB2312Encoding : public Encoding {
    using self = GB2312Encoding;
    using super = Encoding;

public:
    int byte_size(const char* data) const override {
        (void)data; // 告诉编译器可能不使用data
        return 0;
    }

    self* clone() const override {
        return new self{};
    }

    CString __str__() const override {
        return GB2312;
    }
};

/**
 * @brief 编码名-编码方式 映射
 */
def encoding_map(const CString& encodingName)->Encoding* {
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