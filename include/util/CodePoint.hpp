/**
 * @brief 码点，字符抽象
 * @author Ricky
 * @date 2024/12/19
 * @version 1.0
 */
#ifndef CODE_POINT_HPP
#define CODE_POINT_HPP

#include "Encoding.hpp"
#include "Vec.hpp"
#include "Dict.hpp"

#include <mutex>
#include <shared_mutex>

namespace my::util {

class CodePoint : public Object<CodePoint> {
public:
    using Self = CodePoint;
    using Super = Object<Self>;

    static const Array<CodePoint> BLANK;
    static const Array<CodePoint> DIGIT;
    static const Array<CodePoint> LOWER_CASE_LETTER;
    static const Array<CodePoint> UPPER_CASE_LETTER;

    CodePoint() :
            code_size_(0), byte_code_(nullptr) {}

    CodePoint(char ch) :
            code_size_(sizeof(char)), byte_code_(my_alloc<char>(code_size_)) {
        byte_code_[0] = ch;
    }

    CodePoint(const char* str, Encoding* encoding) :
            code_size_(encoding->byte_size(str)), byte_code_(my_alloc<char>(code_size_)) {
        std::memcpy(data(), str, code_size_);
    }

    CodePoint(const Self& other) :
            code_size_(other.code_size_), byte_code_(my_alloc<char>(this->code_size_)) {
        std::memcpy(this->data(), other.data(), this->code_size_);
    }

    CodePoint(Self&& other) noexcept :
            code_size_(other.code_size_), byte_code_(other.byte_code_) {
        other.code_size_ = 0;
        other.byte_code_ = nullptr;
    }

    Self& operator=(const Self& other) {
        if (this == &other) return *this;

        my_destroy(byte_code_);
        return *my_construct(this, other);
    }

    Self& operator=(Self&& other) noexcept {
        if (this == &other) return *this;

        my_destroy(byte_code_);
        return *my_construct(this, std::move(other));
    }

    Self& operator=(char ch) {
        my_destroy(byte_code_);
        this->code_size_ = sizeof(char);
        this->byte_code_ = my_alloc<char>(code_size_);
        byte_code_[0] = ch;
        return *this;
    }

    ~CodePoint() {
        code_size_ = 0;
        my_delloc(byte_code_);
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
    usize size() const noexcept {
        return usize(code_size_);
    }

    operator char() const {
        return byte_code_[0];
    }

    bool is_ascii() const {
        return u32(byte_code_[0]) < 0x80;
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

    Self upper() const {
        if (is_ascii()) {
            return Self{static_cast<char>(std::toupper(byte_code_[0]))};
        } else {
            io::my_error("Not supported yet.");
            return *this;
        }
    }

    Self lower() const {
        if (is_ascii()) {
            return Self{static_cast<char>(std::tolower(byte_code_[0]))};
        } else {
            io::my_error("Not supported yet.");
            return *this;
        }
    }

    [[nodiscard]] CString __str__() const {
        return CString{data(), size()};
    }

    [[nodiscard]] hash_t __hash__() const {
        return bytes_hash(data(), size());
    }

    [[nodiscard]] cmp_t __cmp__(const Self& other) const {
        usize m_size = this->size(), o_size = other.size();
        if (m_size != o_size) {
            return m_size - o_size;
        }
        return std::memcmp(this->data(), other.data(), m_size);
    }

    [[nodiscard]] bool __equals__(const Self& other) const {
        return this->__cmp__(other) == 0;
    }

    [[nodiscard]] bool __equals__(u8 ch) const {
        return static_cast<u8>(byte_code_[0]) == static_cast<u8>(ch);
    }

private:
    i8 code_size_;    // 字节码长度
    char* byte_code_; // 字节码
};

const Array<CodePoint> CodePoint::BLANK = {' ', '\0', '\t', '\n', '\r', '\v', '\f'};
const Array<CodePoint> CodePoint::DIGIT = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
const Array<CodePoint> CodePoint::LOWER_CASE_LETTER = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
const Array<CodePoint> CodePoint::UPPER_CASE_LETTER = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};

/**
 * @brief 码点内存池
 * @note 性能堪忧，是构造函数的一半，但可以节省内存
 */
class CodePointPool : public Object<CodePointPool> {
public:
    using Self = CodePointPool;
    using Super = Object<Self>;

    static CodePointPool& instance() {
        static std::once_flag once;
        std::call_once(once, [] {
            instance_ = new CodePointPool();
        });
        return *instance_;
    }

    std::shared_ptr<const CodePoint> get(char ch) {
        hash_t hash = hash_t(ch);
        return get_impl(hash, [ch]() { return std::make_shared<CodePoint>(ch); });
    }

    std::shared_ptr<const CodePoint> get(const char* str, Encoding* encoding) {
        i8 codeSize = encoding->byte_size(str);
        hash_t hash = bytes_hash(str, codeSize);
        return get_impl(hash, [str, encoding]() { return std::make_shared<CodePoint>(str, encoding); });
    }

private:
    CodePointPool() = default;

    template <typename F>
    std::shared_ptr<const CodePoint> get_impl(hash_t hash, F&& factory) {
        std::shared_lock lock(mutex_);
        if (pool_.contains(hash)) {
            return pool_.get(hash);
        }
        lock.unlock();

        std::unique_lock unique_lock(mutex_);
        if (pool_.contains(hash)) {
            return pool_.get(hash);
        }

        auto cp = factory();
        pool_.insert(hash, cp);
        return cp;
    }

    static CodePointPool* instance_;

    mutable std::shared_mutex mutex_;
    Dict<hash_t, std::shared_ptr<const CodePoint>> pool_;
};

CodePointPool* CodePointPool::instance_ = nullptr;

/**
 * @brief 获取字符串的所有码点
 */
fn get_code_points(const char* str, usize len, Encoding* encoding)->Vec<CodePoint> {
    Vec<CodePoint> cps;
    usize i = 0;
    while (i < len) {
        cps.append(CodePoint{str + i, encoding});
        i += cps[-1].size();
        if (i > len) EncodingError("Invalid encoding, code point out of range");
    }
    return cps;
}

} // namespace my::util

#endif // CODE_POINT_HPP