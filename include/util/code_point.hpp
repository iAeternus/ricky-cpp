/**
 * @brief 码点，字符抽象
 * @author Ricky
 * @date 2024/12/19
 * @version 1.0
 */
#ifndef CODE_POINT_HPP
#define CODE_POINT_HPP

#include "array.hpp"
#include "encoding.hpp"
#include "vec.hpp"

namespace my::util {

/**
 * @class CodePoint
 * @brief 码点抽象
 */
template <EncodingType Enc = EncodingType::UTF8, typename Alloc = mem::Allocator<char>>
class CodePoint : public Object<CodePoint<Enc>> {
public:
    using Self = CodePoint<Enc>;
    using Super = Object<Self>;

    static const Array<CodePoint<Enc>> BLANK;
    static const Array<CodePoint<Enc>> DIGIT;
    static const Array<CodePoint<Enc>> LOWER_CASE_LETTER;
    static const Array<CodePoint<Enc>> UPPER_CASE_LETTER;

    CodePoint(const Alloc& alloc = Alloc()) :
            alloc_(alloc), code_len_(0), byte_code_(nullptr) {}

    CodePoint(const char ch, const Alloc& alloc = Alloc()) :
            alloc_(alloc), code_len_(sizeof(u8)), byte_code_(alloc_.allocate(code_len_)) {
        byte_code_[0] = ch;
    }

    CodePoint(const char* str, const Alloc& alloc = Alloc()) :
            alloc_(alloc), code_len_(EncodingTraits<Enc>::char_size(str)), byte_code_(alloc_.allocate(code_len_)) {
        std::memcpy(data(), str, code_len_);
    }

    CodePoint(const Self& other) :
            alloc_(other.alloc_), code_len_(other.code_len_), byte_code_(alloc_.allocate(this->code_len_)) {
        std::memcpy(this->data(), other.data(), this->code_len_);
    }

    CodePoint(Self&& other) noexcept :
            alloc_(other.alloc_), code_len_(other.code_len_), byte_code_(other.byte_code_) {
        other.code_len_ = 0;
        other.byte_code_ = nullptr;
    }

    Self& operator=(const Self& other) {
        if (static_cast<const void*>(this) == static_cast<const void*>(&other)) {
            return *this;
        }

        alloc_.destroy(byte_code_);
        alloc_.construct(this, other);
        return *this;
    }

    Self& operator=(Self&& other) noexcept {
        if (static_cast<const void*>(this) == static_cast<const void*>(&other)) {
            return *this;
        }

        alloc_.destroy(byte_code_);
        alloc_.construct(this, std::move(other));
        return *this;
    }

    Self& operator=(const char ch) {
        alloc_.destroy(byte_code_);
        this->code_len_ = sizeof(u8);
        this->byte_code_ = alloc_.allocate(code_len_);
        byte_code_[0] = ch;
        return *this;
    }

    ~CodePoint() {
        if (byte_code_ != nullptr) {
            alloc_.deallocate(byte_code_, code_len_);
            byte_code_ = nullptr;
            code_len_ = 0;
        }
    }

    /**
     * @brief 返回字节码
     */
    char* data() noexcept {
        return byte_code_;
    }

    const char* data() const noexcept {
        return byte_code_;
    }

    /**
     * @brief 字节码长度
     */
    usize len() const noexcept {
        return code_len_;
    }

    /**
     * @brief 获取编码类型
     */
    static constexpr EncodingType encoding() noexcept {
        return Enc;
    }

    operator char() const {
        return byte_code_[0];
    }

    bool is_ascii() const {
        return static_cast<u32>(byte_code_[0]) < 0x80;
    }

    bool is_blank() const {
        return is_ascii() && BLANK.contains(*this);
    }

    bool is_alpha() const {
        return is_ascii() && (UPPER_CASE_LETTER.contains(*this) || LOWER_CASE_LETTER.contains(*this));
    }

    bool is_digit() const {
        return is_ascii() && DIGIT.contains(*this);
    }

    bool is_upper() const {
        return is_ascii() && UPPER_CASE_LETTER.contains(*this);
    }

    bool is_lower() const {
        return is_ascii() && LOWER_CASE_LETTER.contains(*this);
    }

    /**
     * @brief 转换为大写ascii字符
     * @return 大写ascii字符
     * @exception Exception 若码点非ascii字符，则抛出 runtime_exception
     */
    Self upper() const {
        if (!is_ascii()) {
            throw runtime_exception("Not supported yet.");
        }
        return Self{static_cast<char>(std::toupper(byte_code_[0]))};
    }

    /**
     * @brief 转换为小写ascii字符
     * @return 小写ascii字符
     * @exception Exception 若码点非ascii字符，则抛出 runtime_exception
     */
    Self lower() const {
        if (!is_ascii()) {
            throw runtime_exception("not supported yet.");
        }
        return Self{static_cast<char>(std::tolower(byte_code_[0]))};
    }

    [[nodiscard]] CString __str__() const {
        return CString{data(), len()};
    }

    [[nodiscard]] hash_t __hash__() const {
        return bytes_hash(data(), len());
    }

    [[nodiscard]] cmp_t __cmp__(const Self& other) const {
        const usize m_size = this->len(), o_size = other.len();
        if (m_size != o_size) {
            return static_cast<cmp_t>(m_size) - static_cast<cmp_t>(o_size);
        }
        return std::memcmp(this->data(), other.data(), m_size);
    }

    [[nodiscard]] bool __equals__(const Self& other) const {
        return this->__cmp__(other) == 0;
    }

    [[nodiscard]] bool __equals__(const u8 ch) const {
        return static_cast<u8>(byte_code_[0]) == ch;
    }

private:
    Alloc alloc_{};   // 内存分配器
    u8 code_len_;     // 字节码长度
    char* byte_code_; // 字节码
};

template <EncodingType Enc, typename Alloc>
inline const Array<CodePoint<Enc>> CodePoint<Enc, Alloc>::BLANK = {' ', '\0', '\t', '\n', '\r', '\v', '\f'};
template <EncodingType Enc, typename Alloc>
inline const Array<CodePoint<Enc>> CodePoint<Enc, Alloc>::DIGIT = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
template <EncodingType Enc, typename Alloc>
inline const Array<CodePoint<Enc>> CodePoint<Enc, Alloc>::LOWER_CASE_LETTER = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
template <EncodingType Enc, typename Alloc>
inline const Array<CodePoint<Enc>> CodePoint<Enc, Alloc>::UPPER_CASE_LETTER = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};

/**
 * @brief 获取字符串的所有码点
 * @exception Exception 若码点越界，说明存在非法编码的码点，则抛出 runtime_exception
 */
template <EncodingType Enc, typename Alloc = mem::Allocator<char>>
Vec<CodePoint<Enc, Alloc>> get_code_points(const char* str, const usize length, const Alloc& alloc = Alloc()) {
    Vec<CodePoint<Enc, Alloc>> cps;
    usize i = 0;
    while (i < length) {
        const auto code_size = EncodingTraits<Enc>::char_size(str + i);
        if (code_size == 1) {
            cps.push(CodePoint<Enc, Alloc>(str[i], alloc));
            i += 1;
        } else {
            cps.push(CodePoint<Enc, Alloc>(str + i, alloc));
            i += code_size;
        }
    }
    return cps;
}

} // namespace my::util

/**
 * @brief 为CodePoint提供format支持
 */
template <my::util::EncodingType Enc, typename Alloc>
struct std::formatter<my::util::CodePoint<Enc, Alloc>> {
    constexpr auto parse(format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const my::util::CodePoint<Enc, Alloc>& cp, format_context& ctx) const -> format_context::iterator {
        return std::format_to(ctx.out(), "{}", cp.__str__());
    }
};

#endif // CODE_POINT_HPP