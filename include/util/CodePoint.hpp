/**
 * @brief 码点，字符抽象
 * @author Ricky
 * @date 2024/12/19
 * @version 1.0
 */
#ifndef CODE_POINT_HPP
#define CODE_POINT_HPP

#include "Encoding.hpp"
#include "Vector.hpp"
#include "Dict.hpp"

#include <mutex>
#include <shared_mutex>

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
        other.byteCode_ = nullptr;
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
        my_destroy(byteCode_);
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
    isize size() const noexcept {
        return isize(codeSize_);
    }

    operator char() const {
        return byteCode_[0];
    }

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

    self upper() const {
        if (isAscii()) {
            return self{static_cast<char>(std::toupper(byteCode_[0]))};
        } else {
            io::my_error("Not supported yet.");
            return *this;
        }
    }

    self lower() const {
        if (isAscii()) {
            return self{static_cast<char>(std::tolower(byteCode_[0]))};
        } else {
            io::my_error("Not supported yet.");
            return *this;
        }
    }

    CString __str__() const {
        return CString{data(), size()};
    }

    hash_t __hash__() const {
        return bytes_hash(data(), size());
    }

    cmp_t __cmp__(const self& other) const {
        isize m_size = this->size(), o_size = other.size();
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

const Array<CodePoint> CodePoint::BLANK = {' ', '\0', '\t', '\n', '\r', '\v', '\f'};
const Array<CodePoint> CodePoint::DIGIT = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
const Array<CodePoint> CodePoint::LOWER_CASE_LETTER = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
const Array<CodePoint> CodePoint::UPPER_CASE_LETTER = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};

/**
 * @brief 码点内存池
 * @note 性能堪忧，是构造函数的一半，但可以节省内存
 */
class CodePointPool : public Object<CodePointPool> {
    using self = CodePointPool;
    using super = Object<self>;

public:
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

        std::unique_lock uniqueLock(mutex_);
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
fn getCodePoints(const char* str, isize len, Encoding* encoding)->Vector<CodePoint> {
    Vector<CodePoint> cps;
    i32 i = 0;
    while (i < len) {
        cps.append(CodePoint{str + i, encoding});
        i += cps[-1].size();
        if (i > len) EncodingError("Invalid encoding, code point out of range");
    }
    return cps;
}

} // namespace my::util

#endif // CODE_POINT_HPP