#include "string.hpp"

namespace my::str {

namespace {

void max_suffix(const u8* x, const usize m, const bool order, usize& pos, usize& period) {
    std::ptrdiff_t ms = -1;
    usize j = 0;
    usize k = 1;
    usize p = 1;

    while (j + k < m) {
        const u8 a = x[j + k];
        const u8 b = x[static_cast<usize>(ms + static_cast<std::ptrdiff_t>(k))];

        if (a == b) {
            if (k == p) {
                j += p;
                k = 1;
            } else {
                ++k;
            }
        } else if ((a > b) == order) {
            j += k;
            k = 1;
            p = static_cast<usize>(static_cast<std::ptrdiff_t>(j) - ms);
        } else {
            ms = static_cast<std::ptrdiff_t>(j);
            j = static_cast<usize>(ms + 1);
            k = 1;
            p = 1;
        }
    }

    pos = (ms < 0) ? 0 : static_cast<usize>(ms);
    period = p;
}

Option<usize> twoway_find(const u8* hay, const usize hlen, const u8* pat, const usize plen) {
    if (plen == 0) return Option<usize>::Some(0);
    if (plen > hlen) return Option<usize>::None();

    usize ms1 = 0, p1 = 0;
    usize ms2 = 0, p2 = 0;
    max_suffix(pat, plen, true, ms1, p1);
    max_suffix(pat, plen, false, ms2, p2);

    usize crit;
    usize period;
    if (ms1 > ms2) {
        crit = ms1;
        period = p1;
    } else {
        crit = ms2;
        period = p2;
    }

    usize pos = 0;
    bool is_periodic = false;
    if (period > 0 && period + crit < plen) {
        is_periodic = (std::memcmp(pat, pat + period, crit + 1) == 0);
    }
    if (is_periodic) {
        usize memory = 0;
        while (pos + plen <= hlen) {
            usize i = std::max(crit, memory);
            while (i < plen && pat[i] == hay[pos + i]) {
                ++i;
            }
            if (i >= plen) {
                i = crit;
                while (i > memory && pat[i - 1] == hay[pos + i - 1]) {
                    --i;
                }
                if (i <= memory) {
                    return Option<usize>::Some(pos);
                }
                pos += period;
                memory = plen - period;
            } else {
                pos += (i > crit) ? (i - crit) : 1;
                memory = 0;
            }
        }
    } else {
        while (pos + plen <= hlen) {
            usize i = crit + 1;
            while (i < plen && pat[i] == hay[pos + i]) {
                ++i;
            }
            if (i >= plen) {
                i = crit;
                while (i > 0 && pat[i - 1] == hay[pos + i - 1]) {
                    --i;
                }
                if (i == 0) {
                    return Option<usize>::Some(pos);
                }
                pos += i + 1;
            } else {
                pos += i - crit;
            }
        }
    }

    return Option<usize>::None();
}

} // namespace

StringView::StringView(const char* s) :
        data_(reinterpret_cast<const u8*>(s)),
        len_(s ? static_cast<usize>(std::strlen(s)) : 0) {
    detail::validate_utf8(data_, len_);
}

StringView::StringView(const char* s, const usize len) :
        data_(reinterpret_cast<const u8*>(s)), len_(len) {
    detail::validate_utf8(data_, len_);
}

StringView::StringView(const u8* s, const usize len) :
        data_(s), len_(len) {
    detail::validate_utf8(data_, len_);
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
