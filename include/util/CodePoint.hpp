/**
 * @brief 码点，字符抽象
 * @author Ricky
 * @date 2024/12/19
 * @version 1.0
 */
#ifndef CODE_POINT_HPP
#define CODE_POINT_HPP

#include "Encoding.hpp"
#include "DynArray.hpp"

namespace my::util {

class CodePoint : public Object<CodePoint> {
    using self = CodePoint;
    using super = Object<self>;

public:
    static const Array<CodePoint> BLANK;
    static const Array<CodePoint> DIGIT;
    static const Array<CodePoint> LOWER_CASE_LETTER;
    static const Array<CodePoint> UPPER_CASE_LETTER;

    CodePoint() :
            codeSize_(0), byteCode_(nullptr) {}

    CodePoint(char ch) :
            codeSize_(sizeof(char)), byteCode_(my_alloc<char>(codeSize_)) {
        byteCode_[0] = ch;
    }

    CodePoint(const char* str, Encoding* encoding) :
            codeSize_(encoding->byte_size(str)), byteCode_(my_alloc<char>(codeSize_)) {
        std::memcpy(data(), str, codeSize_);
    }

    CodePoint(const self& other) :
            codeSize_(other.codeSize_), byteCode_(my_alloc<char>(this->codeSize_)) {
        std::memcpy(this->data(), other.data(), this->codeSize_);
    }

    CodePoint(self&& other) noexcept :
            codeSize_(other.codeSize_), byteCode_(other.byteCode_) {
        other.codeSize_ = 0;
        my_delloc(other.byteCode_);
    }

    self& operator=(const self& other) {
        if (this == &other) return *this;

        my_destroy(byteCode_);
        return *my_construct(this, other);
    }

    self& operator=(self&& other) noexcept {
        if (this == &other) return *this;

        my_destroy(byteCode_);
        return *my_construct(this, std::move(other));
    }

    self& operator=(char ch) {
        my_delloc(byteCode_);
        this->codeSize_ = sizeof(char);
        this->byteCode_ = my_alloc<char>(codeSize_);
        byteCode_[0] = ch;
        return *this;
    }

    ~CodePoint() {
        codeSize_ = 0;
        my_delloc(byteCode_);
    }

    /**
     * @brief 返回字节码
     */
    char* data() noexcept {
        return byteCode_;
    }

    const char* data() const noexcept {
        return byteCode_;
    }

    /**
     * @brief 字节码长度
     */
    c_size size() const noexcept {
        return c_size(codeSize_);
    }

    operator char() const {
        return byteCode_[0];
    }

    self upper() const {}

    self lower() const {}

    bool isAscii() const {
        return u32(byteCode_[0]) < 0x80;
    }

    bool isBlank() const {
        return isAscii() && BLANK.contains(*this);
    }

    bool isAlpha() const {
        return isAscii() && (UPPER_CASE_LETTER.contains(*this) || LOWER_CASE_LETTER.contains(*this));
    }

    bool isDigit() const {
        return isAscii() && DIGIT.contains(*this);
    }

    bool isUpper() const {
        return isAscii() && UPPER_CASE_LETTER.contains(*this);
    }

    bool isLower() const {
        return isAscii() && LOWER_CASE_LETTER.contains(*this);
    }

    CString __str__() const {
        return CString{data(), size()};
    }

    hash_t __hash__() const {
        return bytes_hash(data(), size());
    }

    cmp_t __cmp__(const self& other) const {
        c_size m_size = this->size(), o_size = other.size();
        if (m_size != o_size) {
            return m_size - o_size;
        }
        return std::memcmp(this->data(), other.data(), m_size);
    }

    bool __equals__(const self& other) const {
        return this->__cmp__(other) == 0;
    }

private:
    i8 codeSize_;    // 字节码长度
    char* byteCode_; // 字节码
};

const Array<CodePoint> CodePoint::BLANK = {'\0', '\t', '\n', '\r', '\v', '\f', ' '};
const Array<CodePoint> CodePoint::DIGIT = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
const Array<CodePoint> CodePoint::LOWER_CASE_LETTER = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
const Array<CodePoint> CodePoint::UPPER_CASE_LETTER = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};

/**
 * @brief 获取字符串的所有码点
 */
def getCodePoints(const char* str, c_size len, Encoding* encoding)->DynArray<CodePoint> {
    DynArray<CodePoint> cps;
    c_size i = 0LL;
    while (i < len) {
        cps.append(CodePoint{str + i, encoding});
        i += cps.at(cps.size() - 1).size();
        if (i > len) EncodingError("Invalid encoding, code point out of range");
    }
    return cps;
}

} // namespace my::util

#endif // CODE_POINT_HPP