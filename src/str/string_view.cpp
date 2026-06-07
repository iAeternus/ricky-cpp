#include "string.hpp"
#include "string_algorithm.hpp"

namespace my::str {

namespace detail {

bool is_ascii_whitespace(const char32_t cp) {
    if (cp > 0x7Fu) return false;
    return std::isspace(static_cast<unsigned char>(cp)) != 0;
}

void validate_utf8(const u8* data, const usize len) {
    if (len == 0) return;
    if (data == nullptr) {
        throw runtime_exception("Invalid UTF-8: null pointer");
    }
    const u8* p = data;
    const u8* end = data + len;
    char32_t cp = 0;
    while (p < end) {
        if (!detail::decode_next(p, end, cp)) {
            throw runtime_exception("Invalid UTF-8");
        }
    }
}

} // namespace detail

const u8& StringView::at(const usize idx) const {
    if (idx >= len_) {
        throw std::out_of_range("StringView::at: index out of range");
    }
    return data_[idx];
}

StringView::StringView(const char* s) :
        data_(reinterpret_cast<const u8*>(s)),
        len_(s ? static_cast<usize>(std::strlen(s)) : 0),
        cstr_backed_(s != nullptr) {}

StringView::StringView(const char* s, const usize len) :
        data_(reinterpret_cast<const u8*>(s)), len_(len), cstr_backed_(s != nullptr) {}

StringView::StringView(const u8* s, const usize len) :
        data_(s), len_(len), cstr_backed_(false) {}

StringView StringView::from_null_terminated(const char* s, const usize len) noexcept {
    StringView view;
    view.data_ = reinterpret_cast<const u8*>(s);
    view.len_ = len;
    view.cstr_backed_ = (s != nullptr);
    return view;
}

usize StringView::len() const noexcept {
    return len_;
}

bool StringView::is_empty() const noexcept {
    return len_ == 0;
}

const u8* StringView::as_bytes() const noexcept {
    return data_;
}

StringView StringView::as_str() const noexcept {
    return *this;
}

std::string_view StringView::to_std_string_view() const noexcept {
    return std::string_view(reinterpret_cast<const char*>(data_), len_);
}

std::string StringView::to_std_string() const {
    return std::string(reinterpret_cast<const char*>(data_), len_);
}

const u8& StringView::operator[](const usize idx) const noexcept {
    return data_[idx];
}

StringView StringView::slice(const usize start, const usize end) const noexcept {
    if (start > len_ || end > len_ || start > end) {
        return StringView{};
    }
    return StringView(data_ + start, end - start);
}

StringView StringView::slice(const usize start) const noexcept {
    return slice(start, len_);
}

void StringView::CStrDeleter::operator()(char* p) noexcept {
    if (!p) return;
    alloc.deallocate(p, size);
}

auto StringView::operator==(const Self& other) const -> bool {
    return eq(other);
}

auto StringView::operator!=(const Self& other) const -> bool {
    return !eq(other);
}

auto StringView::operator<(const Self& other) const -> bool {
    return cmp(other) < 0;
}

auto StringView::operator<=(const Self& other) const -> bool {
    return cmp(other) <= 0;
}

auto StringView::operator>(const Self& other) const -> bool {
    return cmp(other) > 0;
}

auto StringView::operator>=(const Self& other) const -> bool {
    return cmp(other) >= 0;
}

StringView::CharsRange::Iterator::Iterator(const u8* cur, const u8* end_) :
        p(cur), end(end_), next(cur), value(0) {
    if (p < end) {
        const u8* tmp = p;
        if (!detail::decode_next(tmp, end, value)) {
            throw runtime_exception("Invalid UTF-8");
        }
        next = tmp;
    }
}

StringView::CharsRange::Iterator& StringView::CharsRange::Iterator::operator++() {
    p = next;
    if (p < end) {
        const u8* tmp = p;
        if (!detail::decode_next(tmp, end, value)) {
            throw runtime_exception("Invalid UTF-8");
        }
        next = tmp;
    }
    return *this;
}

usize StringView::CharsRange::count() const {
    usize cnt = 0;
    const u8* p = begin_;
    const u8* end = end_;
    char32_t cp = 0;
    while (p < end) {
        if (!detail::decode_next(p, end, cp)) {
            throw runtime_exception("Invalid UTF-8");
        }
        ++cnt;
    }
    return cnt;
}

Option<char32_t> StringView::CharsRange::nth(const usize idx) const {
    usize i = 0;
    const u8* p = begin_;
    const u8* end = end_;
    char32_t cp = 0;
    while (p < end) {
        if (!detail::decode_next(p, end, cp)) {
            throw runtime_exception("Invalid UTF-8");
        }
        if (i == idx) {
            return Option<char32_t>::Some(cp);
        }
        ++i;
    }
    return Option<char32_t>::None();
}

Option<char32_t> StringView::CharsRange::last() const {
    const u8* p = begin_;
    const u8* end = end_;
    char32_t cp = 0;
    bool found = false;
    while (p < end) {
        if (!detail::decode_next(p, end, cp)) {
            throw runtime_exception("Invalid UTF-8");
        }
        found = true;
    }
    if (!found) {
        return Option<char32_t>::None();
    }
    return Option<char32_t>::Some(cp);
}

StringView::CharsRange::EnumerateRange::Iterator::Iterator(const u8* cur, const u8* end_) :
        p(cur), end(end_), next(cur), value(0), idx(0) {
    if (p < end) {
        const u8* tmp = p;
        if (!detail::decode_next(tmp, end, value)) {
            throw runtime_exception("Invalid UTF-8");
        }
        next = tmp;
    }
}

StringView::CharsRange::EnumerateRange::Iterator& StringView::CharsRange::EnumerateRange::Iterator::operator++() {
    p = next;
    ++idx;
    if (p < end) {
        const u8* tmp = p;
        if (!detail::decode_next(tmp, end, value)) {
            throw runtime_exception("Invalid UTF-8");
        }
        next = tmp;
    }
    return *this;
}

StringView::BytesRange StringView::bytes() const {
    return {data_, data_ + len_};
}

const char* StringView::as_cstr() const noexcept {
    if (len_ == 0) {
        return "";
    }
    if (cstr_backed_) {
        return reinterpret_cast<const char*>(data_);
    }
    return nullptr;
}

StringView::CStrPtr StringView::into_cstr() const {
    const usize size = len_ + 1;
    cstr_allocator alloc{};
    char* out = alloc.allocate(size);
    if (len_ != 0) {
        std::memcpy(out, data_, len_);
    }
    out[len_] = '\0';
    return CStrPtr(out, CStrDeleter{alloc, size});
}

StringView::CharsRange StringView::chars() const {
    return {data_, data_ + len_};
}

Option<usize> StringView::find(const StringView& pat) const {
    if (pat.len_ == 0) return Option<usize>::Some(0);
    if (pat.len_ > len_) return Option<usize>::None();
    if (pat.len_ == 1) {
        const void* res = std::memchr(data_, pat.data_[0], len_);
        if (!res) return Option<usize>::None();
        return Option<usize>::Some(static_cast<usize>(static_cast<const u8*>(res) - data_));
    }
    return twoway_find(data_, len_, pat.data_, pat.len_);
}

bool StringView::contains(const StringView& pat) const {
    return find(pat).is_some();
}

bool StringView::starts_with(const StringView& prefix) const {
    if (prefix.len_ > len_) return false;
    return std::memcmp(data_, prefix.data_, prefix.len_) == 0;
}

bool StringView::ends_with(const StringView& suffix) const {
    if (suffix.len_ > len_) return false;
    return std::memcmp(data_ + len_ - suffix.len_, suffix.data_, suffix.len_) == 0;
}

Option<StringView> StringView::strip_prefix(const StringView& prefix) const {
    if (!starts_with(prefix)) {
        return Option<StringView>::None();
    }
    return Option<StringView>::Some(StringView(data_ + prefix.len_, len_ - prefix.len_));
}

Option<StringView> StringView::strip_suffix(const StringView& suffix) const {
    if (!ends_with(suffix)) {
        return Option<StringView>::None();
    }
    return Option<StringView>::Some(StringView(data_, len_ - suffix.len_));
}

util::Vec<StringView> StringView::split(const StringView& pat) const {
    util::Vec<StringView> out;
    if (pat.len_ == 0) {
        out.push(StringView(data_, 0));
        const u8* p = data_;
        const u8* end = data_ + len_;
        while (p < end) {
            const u8* start = p;
            char32_t cp = 0;
            if (!detail::decode_next(p, end, cp)) {
                throw runtime_exception("Invalid UTF-8");
            }
            out.push(StringView(start, static_cast<usize>(p - start)));
        }
        out.push(StringView(data_ + len_, 0));
        return out;
    }

    usize start = 0;
    for (usize i = 0; i + pat.len_ <= len_; ++i) {
        if (std::memcmp(data_ + i, pat.data_, pat.len_) == 0) {
            out.push(StringView(data_ + start, i - start));
            start = i + pat.len_;
            i = start == 0 ? 0 : start - 1;
        }
    }
    out.push(StringView(data_ + start, len_ - start));
    return out;
}

util::Vec<StringView> StringView::split_whitespace() const {
    util::Vec<StringView> out;
    const u8* p = data_;
    const u8* end = data_ + len_;
    while (p < end) {
        const u8* start = p;
        char32_t cp = 0;
        if (!detail::decode_next(p, end, cp)) {
            throw runtime_exception("Invalid UTF-8");
        }
        if (!detail::is_ascii_whitespace(cp)) {
            const u8* token_start = start;
            const u8* token_end = p;
            while (p < end) {
                const u8* cur = p;
                if (!detail::decode_next(p, end, cp)) {
                    throw runtime_exception("Invalid UTF-8");
                }
                if (detail::is_ascii_whitespace(cp)) {
                    token_end = cur;
                    break;
                }
                token_end = p;
            }
            out.push(StringView(token_start, static_cast<usize>(token_end - token_start)));
        }
    }
    return out;
}

util::Vec<StringView> StringView::lines() const {
    util::Vec<StringView> out;
    usize start = 0;
    usize i = 0;
    while (i < len_) {
        if (data_[i] == '\n') {
            usize end = i;
            if (end > start && data_[end - 1] == '\r') {
                --end;
            }
            out.push(StringView(data_ + start, end - start));
            start = i + 1;
        }
        ++i;
    }
    if (start < len_) {
        out.push(StringView(data_ + start, len_ - start));
    }
    return out;
}

StringView StringView::trim() const {
    if (len_ == 0) return *this;

    const u8* p = data_;
    const u8* end = data_ + len_;
    const u8* start = data_;
    const u8* last_non_ws_end = data_;

    while (p < end) {
        const u8* cur = p;
        char32_t cp = 0;
        if (!detail::decode_next(p, end, cp)) {
            throw runtime_exception("Invalid UTF-8");
        }
        if (!detail::is_ascii_whitespace(cp)) {
            if (start == data_) start = cur;
            last_non_ws_end = p;
        }
    }

    if (last_non_ws_end == data_) {
        return StringView(data_, 0);
    }
    return StringView(start, static_cast<usize>(last_non_ws_end - start));
}

String<mem::Allocator<u8>> StringView::to_string() const {
    return String<mem::Allocator<u8>>(*this);
}

auto StringView::hash() const -> hash_t {
    return bytes_hash(reinterpret_cast<const char*>(data_), len_);
}

auto StringView::cmp(const Self& other) const -> cmp_t {
    const usize min_len = std::min(len_, other.len_);
    if (min_len > 0) {
        const auto rc = std::memcmp(data_, other.data_, min_len);
        if (rc != 0) return static_cast<cmp_t>(rc);
    }
    return static_cast<cmp_t>(len_) - static_cast<cmp_t>(other.len_);
}

auto StringView::eq(const Self& other) const -> bool {
    return cmp(other) == 0;
}

String<mem::Allocator<u8>> StringView::replace(const StringView& from, const StringView& to) const {
    if (from.len() == 0) {
        util::Vec<StringView> parts = split(from);
        String<mem::Allocator<u8>> res;
        for (usize i = 0; i < parts.len(); ++i) {
            if (i > 0) res.push_str(to);
            res.push_str(parts.at(i));
        }
        return res;
    }

    String<mem::Allocator<u8>> res;
    usize start = 0;
    for (usize i = 0; i + from.len() <= len_; ++i) {
        if (std::memcmp(data_ + i, from.data_, from.len_) == 0) {
            if (i > start) {
                res.push_str(StringView(data_ + start, i - start));
            }
            res.push_str(to);
            start = i + from.len();
            i = start == 0 ? 0 : start - 1;
        }
    }
    if (start < len_) {
        res.push_str(StringView(data_ + start, len_ - start));
    }
    return res;
}

String<mem::Allocator<u8>> StringView::to_lowercase() const {
    String<mem::Allocator<u8>> res;
    const u8* p = data_;
    const u8* end = data_ + len_;
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

String<mem::Allocator<u8>> StringView::to_uppercase() const {
    String<mem::Allocator<u8>> res;
    const u8* p = data_;
    const u8* end = data_ + len_;
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

} // namespace my::str
