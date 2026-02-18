#ifndef STR_STRING_VIEW_HPP
#define STR_STRING_VIEW_HPP

#include "my_pair.hpp"
#include "option.hpp"
#include "vec.hpp"

namespace my::str {

template <typename Alloc>
class String;

namespace detail {

inline bool is_valid_scalar(const u32 cp) {
    return cp <= 0x10FFFFu && !(cp >= 0xD800u && cp <= 0xDFFFu);
}

inline usize utf8_char_len(const u8 lead) {
    if (lead < 0x80u) return 1;
    if ((lead >> 5) == 0x6) return 2;
    if ((lead >> 4) == 0xE) return 3;
    if ((lead >> 3) == 0x1E) return 4;
    return 0;
}

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

inline void validate_utf8(const u8* data, const usize len) {
    if (len == 0) return;
    if (data == nullptr) {
        throw runtime_exception("Invalid UTF-8: null pointer");
    }
    const u8* p = data;
    const u8* end = data + len;
    char32_t cp = 0;
    while (p < end) {
        if (!decode_next(p, end, cp)) {
            throw runtime_exception("Invalid UTF-8");
        }
    }
}

inline bool is_ascii_whitespace(const char32_t cp) {
    if (cp > 0x7Fu) return false;
    return std::isspace(static_cast<unsigned char>(cp)) != 0;
}

} // namespace detail

class StringView {
public:
    using value_type = u8;
    static constexpr usize npos = static_cast<usize>(-1);

    constexpr StringView() noexcept = default;

    explicit StringView(const char* s);
    StringView(const char* s, const usize len);
    StringView(const u8* s, const usize len);

    [[nodiscard]] constexpr usize len() const noexcept { return len_; }
    [[nodiscard]] constexpr bool is_empty() const noexcept { return len_ == 0; }
    [[nodiscard]] constexpr const u8* as_bytes() const noexcept { return data_; }
    [[nodiscard]] constexpr StringView as_str() const noexcept { return *this; }

    struct BytesRange {
        const u8* begin_;
        const u8* end_;

        struct Iterator {
            const u8* p;

            bool operator!=(const Iterator& other) const { return p != other.p; }
            u8 operator*() const { return *p; }
            Iterator& operator++() {
                ++p;
                return *this;
            }
        };

        Iterator begin() const { return {begin_}; }
        Iterator end() const { return {end_}; }

        usize count() const { return static_cast<usize>(end_ - begin_); }
        usize size() const { return count(); }
        Option<u8> nth(const usize idx) const {
            const auto n = count();
            if (idx >= n) return Option<u8>::None();
            return Option<u8>::Some(static_cast<u8>(begin_[idx]));
        }
        Option<u8> last() const {
            const auto n = count();
            if (n == 0) return Option<u8>::None();
            return Option<u8>::Some(static_cast<u8>(begin_[n - 1]));
        }

        struct EnumerateRange {
            const u8* begin_;
            const u8* end_;

            struct Iterator {
                const u8* p;
                usize idx;

                bool operator!=(const Iterator& other) const { return p != other.p; }
                Pair<usize, u8> operator*() const { return {idx, *p}; }
                Iterator& operator++() {
                    ++p;
                    ++idx;
                    return *this;
                }
            };

            Iterator begin() const { return {begin_, 0}; }
            Iterator end() const { return {end_, static_cast<usize>(end_ - begin_)}; }
        };

        EnumerateRange enumerate() const { return {begin_, end_}; }
    };

    struct CharsRange {
        const u8* begin_;
        const u8* end_;

        struct Iterator {
            const u8* p;
            const u8* end;
            const u8* next;
            char32_t value;

            Iterator(const u8* cur, const u8* end_);
            bool operator!=(const Iterator& other) const { return p != other.p; }
            char32_t operator*() const { return value; }
            Iterator& operator++();
        };

        Iterator begin() const { return {begin_, end_}; }
        Iterator end() const { return {end_, end_}; }

        usize count() const;
        usize size() const { return count(); }
        Option<char32_t> nth(usize idx) const;
        Option<char32_t> last() const;

        struct EnumerateRange {
            const u8* begin_;
            const u8* end_;

            struct Iterator {
                const u8* p;
                const u8* end;
                const u8* next;
                char32_t value;
                usize idx;

                Iterator(const u8* cur, const u8* end_);
                bool operator!=(const Iterator& other) const { return p != other.p; }
                Pair<usize, char32_t> operator*() const { return {idx, value}; }
                Iterator& operator++();
            };

            Iterator begin() const { return {begin_, end_}; }
            Iterator end() const { return {end_, end_}; }
        };

        EnumerateRange enumerate() const { return {begin_, end_}; }
    };

    BytesRange bytes() const;
    CharsRange chars() const;

    [[nodiscard]] Option<usize> find(const StringView& pat) const;
    [[nodiscard]] bool contains(const StringView& pat) const;
    [[nodiscard]] bool starts_with(const StringView& prefix) const;
    [[nodiscard]] bool ends_with(const StringView& suffix) const;
    [[nodiscard]] Option<StringView> strip_prefix(const StringView& prefix) const;
    [[nodiscard]] Option<StringView> strip_suffix(const StringView& suffix) const;

    util::Vec<StringView> split(const StringView& pat) const;
    util::Vec<StringView> split_whitespace() const;
    util::Vec<StringView> lines() const;
    StringView trim() const;

    String<mem::Allocator<u8>> replace(const StringView& from, const StringView& to) const;
    String<mem::Allocator<u8>> to_lowercase() const;
    String<mem::Allocator<u8>> to_uppercase() const;
    String<mem::Allocator<u8>> to_string() const;

private:
    const u8* data_{nullptr};
    usize len_{0};
};

} // namespace my::str

#endif // STR_STRING_VIEW_HPP
