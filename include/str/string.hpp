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
class String : public Object<String<Alloc>> {
public:
    using value_type = u8;
    static constexpr usize npos = StringView::npos;
    using Self = String<Alloc>;

    using cstr_allocator = typename Alloc::template rebind<char>::other;
    struct CStrDeleter {
        cstr_allocator alloc{};
        usize size{0};

        void operator()(char* p) noexcept {
            if (!p) return;
            alloc.deallocate(p, size);
        }
    };
    using CStrPtr = std::unique_ptr<char[], CStrDeleter>;

    String() {
        buf_.push(0);
    }

    explicit String(const StringView& view) {
        buf_.reserve(view.len() + 1);
        for (auto b : view.bytes()) {
            buf_.push(b);
        }
        buf_.push(0);
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

    [[nodiscard]] usize len() const noexcept {
        return buf_.len() > 0 ? buf_.len() - 1 : 0;
    }

    [[nodiscard]] bool is_empty() const noexcept {
        return len() == 0;
    }

    [[nodiscard]] usize capacity() const noexcept {
        return buf_.capacity() > 0 ? buf_.capacity() - 1 : 0;
    }

    [[nodiscard]] u8& first() noexcept {
        return buf_.first();
    }

    [[nodiscard]] const u8& first() const noexcept {
        return buf_.first();
    }

    [[nodiscard]] u8& last() noexcept {
        return buf_.at(buf_.len() - 2);
    }

    [[nodiscard]] const u8& last() const noexcept {
        return buf_.at(buf_.len() - 2);
    }

    void reserve(const usize new_cap) {
        buf_.reserve(new_cap + 1);
    }

    void clear() {
        buf_.clear();
        buf_.push(0);
    }

    [[nodiscard]] const u8* as_bytes() const noexcept {
        return buf_.data();
    }

    [[nodiscard]] StringView as_str() const noexcept {
        return StringView(buf_.data(), len());
    }

    [[nodiscard]] const char* as_cstr() const noexcept {
        return reinterpret_cast<const char*>(buf_.data());
    }

    [[nodiscard]] std::string to_std_string() const {
        return std::string(as_cstr(), len());
    }

    [[nodiscard]] String to_string() const {
        return String(*this);
    }

    [[nodiscard]] auto hash() const -> hash_t {
        return bytes_hash(reinterpret_cast<const char*>(as_bytes()), len());
    }

    [[nodiscard]] auto cmp(const Self& other) const -> cmp_t {
        const usize lhs_len = len();
        const usize rhs_len = other.len();
        const usize min_len = std::min(lhs_len, rhs_len);
        if (min_len > 0) {
            const auto rc = std::memcmp(as_bytes(), other.as_bytes(), min_len);
            if (rc != 0) return static_cast<cmp_t>(rc);
        }
        return static_cast<cmp_t>(lhs_len) - static_cast<cmp_t>(rhs_len);
    }

    [[nodiscard]] auto eq(const Self& other) const -> bool {
        return cmp(other) == 0;
    }

    auto operator==(const Self& other) const -> bool { return eq(other); }
    auto operator!=(const Self& other) const -> bool { return !eq(other); }
    auto operator<(const Self& other) const -> bool { return cmp(other) < 0; }
    auto operator<=(const Self& other) const -> bool { return cmp(other) <= 0; }
    auto operator>(const Self& other) const -> bool { return cmp(other) > 0; }
    auto operator>=(const Self& other) const -> bool { return cmp(other) >= 0; }

    [[nodiscard]] u8& operator[](const usize idx) noexcept {
        return buf_[idx];
    }

    [[nodiscard]] const u8& operator[](const usize idx) const noexcept {
        return buf_[idx];
    }

    [[nodiscard]] u8& at(const usize idx) {
        return buf_.at(idx);
    }

    [[nodiscard]] const u8& at(const usize idx) const {
        return buf_.at(idx);
    }

    [[nodiscard]] StringView slice(const usize start, const usize end) const noexcept {
        return as_str().slice(start, end);
    }

    [[nodiscard]] StringView slice(const usize start) const noexcept {
        return as_str().slice(start);
    }

    [[nodiscard]] StringView substr(const usize start, const usize count) const noexcept {
        return as_str().slice(start, start + count);
    }

    [[nodiscard]] CStrPtr into_cstr() const {
        const usize size = len() + 1;
        cstr_allocator alloc{};
        char* out = alloc.allocate(size);
        std::memcpy(out, buf_.data(), size);
        return CStrPtr(out, CStrDeleter{alloc, size});
    }

    void push(const char32_t cp) {
        u8 bytes[4]{};
        const usize n = detail::encode_utf8(cp, bytes);
        buf_.pop();
        buf_.reserve(buf_.len() + n);
        for (usize i = 0; i < n; ++i) {
            buf_.push(bytes[i]);
        }
        buf_.push(0);
    }

    void push_str(const StringView& view) {
        if (view.is_empty()) return;
        buf_.pop();
        buf_.reserve(buf_.len() + view.len());
        for (auto b : view.bytes()) {
            buf_.push(b);
        }
        buf_.push(0);
    }

    Option<char32_t> pop() {
        if (len() == 0) {
            return Option<char32_t>::None();
        }
        usize i = len() - 1;
        while (i > 0 && (buf_.at(i) & 0xC0u) == 0x80u) {
            --i;
        }
        const u8* data = buf_.data();
        const u8* p = data + i;
        const u8* end = data + len();
        char32_t cp = 0;
        if (!detail::decode_next(p, end, cp)) {
            throw runtime_exception("Invalid UTF-8");
        }
        while (buf_.len() > i + 1) {
            buf_.pop();
        }
        buf_.pop();
        buf_.push(0);
        return Option<char32_t>::Some(cp);
    }

    StringView::BytesRange bytes() const {
        return as_str().bytes();
    }

    StringView::CharsRange chars() const {
        return as_str().chars();
    }

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

    [[nodiscard]] util::Vec<StringView> split_whitespace() const {
        return as_str().split_whitespace();
    }

    util::Vec<StringView> lines() const {
        return as_str().lines();
    }

    StringView trim() const {
        return as_str().trim();
    }

    String replace(const StringView& from, const StringView& to) const {
        return as_str().replace(from, to);
    }

    String to_lowercase() const {
        return as_str().to_lowercase();
    }

    String to_uppercase() const {
        return as_str().to_uppercase();
    }

    util::Vec<u8, Alloc> into_bytes() const& {
        return util::Vec<u8, Alloc>(buf_.slice(0, len()));
    }

    util::Vec<u8, Alloc> into_bytes() && {
        auto result = util::Vec<u8, Alloc>(std::move(buf_));
        if (!result.is_empty() && result.at(result.len() - 1) == 0) {
            result.pop();
        }
        return result;
    }

private:
    util::Vec<u8, Alloc> buf_{};
};

template <typename AllocL, typename AllocR>
inline bool operator==(const String<AllocL>& lhs, const String<AllocR>& rhs) {
    return lhs.as_str() == rhs.as_str();
}

template <typename Alloc>
inline bool operator==(const String<Alloc>& lhs, const StringView rhs) {
    return lhs.as_str() == rhs;
}

template <typename Alloc>
inline bool operator==(const StringView lhs, const String<Alloc>& rhs) {
    return lhs == rhs.as_str();
}

template <typename Alloc>
inline bool operator==(const String<Alloc>& lhs, const char* rhs) {
    return lhs.as_str() == rhs;
}

template <typename Alloc>
inline bool operator==(const char* lhs, const String<Alloc>& rhs) {
    return lhs == rhs.as_str();
}

template <typename Alloc>
inline bool operator!=(const String<Alloc>& lhs, const StringView rhs) {
    return !(lhs == rhs);
}

template <typename Alloc>
inline bool operator!=(const StringView lhs, const String<Alloc>& rhs) {
    return !(lhs == rhs);
}

template <typename Alloc>
inline bool operator!=(const String<Alloc>& lhs, const char* rhs) {
    return !(lhs == rhs);
}

template <typename Alloc>
inline bool operator!=(const char* lhs, const String<Alloc>& rhs) {
    return !(lhs == rhs);
}

} // namespace my::str

namespace my {

template <typename T>
concept ToString = requires(const T& t) {
    { to_string(t) } -> std::same_as<str::String<>>;
};

} // namespace my

template <typename Alloc>
struct std::formatter<my::str::String<Alloc>, char> : std::formatter<std::string_view, char> {
    auto format(const my::str::String<Alloc>& value, auto& ctx) const {
        auto view = value.as_str();
        return std::formatter<std::string_view, char>::format(
            std::string_view(reinterpret_cast<const char*>(view.as_bytes()), view.len()), ctx);
    }
};

#endif // STR_STRING_HPP
