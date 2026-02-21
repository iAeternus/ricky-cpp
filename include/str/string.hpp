/**
 * @brief UTF-8 字符串，类似 rust 的 String
 * @author Ricky
 * @date 2026/2/16
 * @version 1.0
 */
#ifndef STR_STRING_HPP
#define STR_STRING_HPP

#include "string_view.hpp"

namespace my::str {

template <typename Alloc = mem::Allocator<u8>>
class String {
public:
    using value_type = u8;
    static constexpr usize npos = StringView::npos;

    String() = default;

    explicit String(const StringView& view) {
        if (!view.is_empty()) {
            buf_.reserve(view.len());
            for (auto b : view.bytes()) {
                buf_.push(b);
            }
        }
    }

    explicit String(const char* s) :
            String(StringView(s)) {}

    String(const char* s, const usize len) :
            String(StringView(s, len)) {}

    String(const String& other) = default;
    String& operator=(const String& other) = default;

    String(String&& other) noexcept = default;
    String& operator=(String&& other) noexcept = default;

    ~String() = default;

    [[nodiscard]] usize len() const noexcept { return buf_.len(); }
    [[nodiscard]] bool is_empty() const noexcept { return buf_.is_empty(); }
    [[nodiscard]] usize capacity() const noexcept { return buf_.capacity(); }

    void reserve(const usize new_cap) { buf_.reserve(new_cap); }
    void clear() { buf_.clear(); }

    [[nodiscard]] const u8* as_bytes() const noexcept { return buf_.data(); }
    [[nodiscard]] StringView as_str() const noexcept { return StringView(buf_.data(), buf_.len()); }

    [[nodiscard]] String to_string() const { return String(*this); }

    void push(const char32_t cp) {
        u8 bytes[4]{};
        const usize n = detail::encode_utf8(cp, bytes);
        buf_.reserve(buf_.len() + n);
        for (usize i = 0; i < n; ++i) {
            buf_.push(bytes[i]);
        }
    }

    void push_str(const StringView& view) {
        if (view.is_empty()) return;
        buf_.reserve(buf_.len() + view.len());
        for (auto b : view.bytes()) {
            buf_.push(b);
        }
    }

    Option<char32_t> pop() {
        if (buf_.is_empty()) {
            return Option<char32_t>::None();
        }
        usize i = buf_.len() - 1;
        while (i > 0 && (buf_.at(i) & 0xC0u) == 0x80u) {
            --i;
        }
        const u8* data = buf_.data();
        const u8* p = data + i;
        const u8* end = data + buf_.len();
        char32_t cp = 0;
        if (!detail::decode_next(p, end, cp)) {
            throw runtime_exception("Invalid UTF-8");
        }
        while (buf_.len() > i) {
            buf_.pop();
        }
        return Option<char32_t>::Some(cp);
    }

    StringView::BytesRange bytes() const { return as_str().bytes(); }
    StringView::CharsRange chars() const { return as_str().chars(); }

    [[nodiscard]] Option<usize> find(const StringView& pat) const {
        return as_str().find(pat);
    }

    [[nodiscard]] bool contains(const StringView& pat) const {
        return as_str().contains(pat);
    }

    [[nodiscard]] bool starts_with(const StringView& prefix) const {
        return as_str().starts_with(prefix);
    }

    [[nodiscard]] bool ends_with(const StringView& suffix) const {
        return as_str().ends_with(suffix);
    }

    [[nodiscard]] Option<StringView> strip_prefix(const StringView& prefix) const {
        return as_str().strip_prefix(prefix);
    }

    [[nodiscard]] Option<StringView> strip_suffix(const StringView& suffix) const {
        return as_str().strip_suffix(suffix);
    }

    util::Vec<StringView> split(const StringView& pat) const {
        return as_str().split(pat);
    }

    util::Vec<StringView> split_whitespace() const {
        return as_str().split_whitespace();
    }

    util::Vec<StringView> lines() const {
        return as_str().lines();
    }

    StringView trim() const {
        return as_str().trim();
    }

    String replace(const StringView& from, const StringView& to) const {
        if (from.len() == 0) {
            util::Vec<StringView> parts = split(from);
            String res;
            for (usize i = 0; i < parts.len(); ++i) {
                if (i > 0) res.push_str(to);
                res.push_str(parts.at(i));
            }
            return res;
        }

        String res;
        const u8* data = buf_.data();
        const usize total = buf_.len();
        usize start = 0;
        for (usize i = 0; i + from.len() <= total; ++i) {
            if (std::memcmp(data + i, from.as_bytes(), from.len()) == 0) {
                if (i > start) {
                    res.push_str(StringView(data + start, i - start));
                }
                res.push_str(to);
                start = i + from.len();
                i = start == 0 ? 0 : start - 1;
            }
        }
        if (start < total) {
            res.push_str(StringView(data + start, total - start));
        }
        return res;
    }

    String to_lowercase() const {
        String res;
        const u8* p = buf_.data();
        const u8* end = p + buf_.len();
        while (p < end) {
            const u8* start = p;
            char32_t cp = 0;
            if (!detail::decode_next(p, end, cp)) {
                throw runtime_exception("Invalid UTF-8");
            }
            if (cp <= 0x7Fu) {
                char c = static_cast<char>(cp);
                c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
                res.push(static_cast<char32_t>(c));
            } else {
                res.push_str(StringView(start, static_cast<usize>(p - start)));
            }
        }
        return res;
    }

    String to_uppercase() const {
        String res;
        const u8* p = buf_.data();
        const u8* end = p + buf_.len();
        while (p < end) {
            const u8* start = p;
            char32_t cp = 0;
            if (!detail::decode_next(p, end, cp)) {
                throw runtime_exception("Invalid UTF-8");
            }
            if (cp <= 0x7Fu) {
                char c = static_cast<char>(cp);
                c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
                res.push(static_cast<char32_t>(c));
            } else {
                res.push_str(StringView(start, static_cast<usize>(p - start)));
            }
        }
        return res;
    }

    util::Vec<u8, Alloc> into_bytes() const& {
        return util::Vec<u8, Alloc>(buf_);
    }

    util::Vec<u8, Alloc> into_bytes() && {
        return std::move(buf_);
    }

private:
    util::Vec<u8, Alloc> buf_{};
};

} // namespace my::str

namespace my {

template <typename T>
concept ToString = requires(const T& t) {
    { to_string(t) } -> std::same_as<str::String<>>;
};

} // namespace my

#endif // STR_STRING_HPP
