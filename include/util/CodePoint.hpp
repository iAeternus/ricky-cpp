/**
 * @brief 码点，字符抽象
 * @author Ricky
 * @date 2024/12/19
 * @version 1.0
 */
#ifndef CODE_POINT_HPP
#define CODE_POINT_HPP

#include "Encoding.hpp"
#include "Dict.hpp"

#include <mutex>
#include <shared_mutex>

namespace my::util {

/**
 * @class CodePoint
 * @brief 码点抽象
 */
template <EncodingType Enc = EncodingType::UTF8>
class CodePoint : public Object<CodePoint<Enc>> {
public:
    using Self = CodePoint<Enc>;
    using Super = Object<Self>;
    using Alloc = Allocator<char>;

    static const Array<Self> BLANK;
    static const Array<Self> DIGIT;
    static const Array<Self> LOWER_CASE_LETTER;
    static const Array<Self> UPPER_CASE_LETTER;

    CodePoint() :
            code_size_(0), byte_code_(nullptr) {}

    CodePoint(const char ch) :
            code_size_(sizeof(u8)), byte_code_(alloc_.allocate(code_size_)) {
        byte_code_[0] = ch;
    }

    CodePoint(const char* str) :
            code_size_(EncodingTraits<Enc>::char_size(str)), byte_code_(alloc_.allocate(code_size_)) {
        std::memcpy(data(), str, code_size_);
    }

    CodePoint(const Self& other) :
            alloc_(other.alloc_), code_size_(other.code_size_), byte_code_(alloc_.allocate(this->code_size_)) {
        std::memcpy(this->data(), other.data(), this->code_size_);
    }

    CodePoint(Self&& other) noexcept :
            alloc_(other.alloc_), code_size_(other.code_size_), byte_code_(other.byte_code_) {
        other.code_size_ = 0;
        other.byte_code_ = nullptr;
    }

    Self& operator=(const Self& other) {
        if (this == &other) return *this;

        alloc_.destroy(byte_code_);
        alloc_.construct(this, other);
        return *this;
    }

    Self& operator=(Self&& other) noexcept {
        if (this == &other) return *this;

        alloc_.destroy(byte_code_);
        alloc_.construct(this, std::move(other));
        return *this;
    }

    Self& operator=(const char ch) {
        alloc_.destroy(byte_code_);
        this->code_size_ = sizeof(u8);
        this->byte_code_ = alloc_.allocate(code_size_);
        byte_code_[0] = ch;
        return *this;
    }

    ~CodePoint() {
        code_size_ = 0;
        alloc_.deallocate(byte_code_, code_size_);
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
        return code_size_;
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
        return CString{data(), size()};
    }

    [[nodiscard]] hash_t __hash__() const {
        return bytes_hash(data(), size());
    }

    [[nodiscard]] cmp_t __cmp__(const Self& other) const {
        const usize m_size = this->size(), o_size = other.size();
        if (m_size != o_size) {
            return m_size - o_size;
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
    u8 code_size_;    // 字节码长度
    char* byte_code_; // 字节码
};

template <EncodingType Enc>
inline const Array<CodePoint<Enc>> CodePoint<Enc>::BLANK = {' ', '\0', '\t', '\n', '\r', '\v', '\f'};
template <EncodingType Enc>
inline const Array<CodePoint<Enc>> CodePoint<Enc>::DIGIT = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
template <EncodingType Enc>
inline const Array<CodePoint<Enc>> CodePoint<Enc>::LOWER_CASE_LETTER = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
template <EncodingType Enc>
inline const Array<CodePoint<Enc>> CodePoint<Enc>::UPPER_CASE_LETTER = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};

/**
 * @brief 码点内存池
 * @note 性能堪忧，是构造函数的一半，但可以节省内存
 */
template <EncodingType Enc = EncodingType::UTF8>
class CodePointPool : public Object<CodePointPool<Enc>> {
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

    // ASCII码点静态池，避免频繁分配和锁竞争
    std::shared_ptr<const CodePoint<Enc>> get(char ch) {
        static std::shared_ptr<const CodePoint<Enc>> ascii_pool[128] = {};
        const auto idx = static_cast<unsigned char>(ch);
        if (idx < 128) {
            if (!ascii_pool[idx]) {
                ascii_pool[idx] = std::make_shared<CodePoint<Enc>>(ch);
            }
            return ascii_pool[idx];
        }
        const auto hash = static_cast<hash_t>(ch);
        return get_impl(hash, [ch]() {
            return std::make_shared<CodePoint<Enc>>(ch);
        });
    }

    std::shared_ptr<const CodePoint<Enc>> get(const char* str) {
        const auto code_size = EncodingTraits<Enc>::char_size(str);
        // 单字节ASCII直接用静态池
        if (code_size == 1 && static_cast<unsigned char>(str[0]) < 128) {
            return get(str[0]);
        }
        const auto hash = bytes_hash(str, code_size);
        return get_impl(hash, [str]() {
            return std::make_shared<CodePoint<Enc>>(str);
        });
    }

private:
    CodePointPool() = default;

    template <typename F>
    std::shared_ptr<const CodePoint<Enc>> get_impl(hash_t hash, F&& factory) {
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

private:
    static CodePointPool* instance_;
    mutable std::shared_mutex mutex_;
    Dict<hash_t, std::shared_ptr<const CodePoint<Enc>>> pool_;
};

template <EncodingType Enc>
inline CodePointPool<Enc>* CodePointPool<Enc>::instance_ = nullptr;

/**
 * @brief 获取字符串的所有码点
 * @exception Exception 若码点越界，说明存在非法编码的码点，则抛出 runtime_exception
 */
template <EncodingType Enc>
fn get_code_points(const char* str, const usize len) -> Vec<CodePoint<Enc>> {
    Vec<CodePoint<Enc>> cps;
    usize i = 0;
    while (i < len) {
        cps.append(*CodePointPool<Enc>::instance().get(str + i));
        i += cps.back().size();
        if (i > len) throw runtime_exception("invalid encoding, code point out of range");
    }
    return cps;
}

} // namespace my::util

#endif // CODE_POINT_HPP