/**
 * @brief 字符串
 * @author Ricky
 * @date 2024/12/19
 * @version 1.0
 */
#ifndef STRING_HPP
#define STRING_HPP

#include "CodePoint.hpp"
#include "NoCopy.hpp"
#include "Object.hpp"

namespace my::util {

class StringManager : public Object<StringManager>, public NoCopy {
    using self = StringManager;

public:
    StringManager(c_size length, CodePoint* sharedHead, Encoding* encoding) :
            length_(length), sharedHead_(sharedHead), encoding_(encoding) {}

    ~StringManager() {
        my_destroy(sharedHead_, length_);
        my_delloc(sharedHead_);
        length_ = 0;
    }

    CodePoint* sharedHead() const {
        return sharedHead_;
    }

    Encoding* encoding() const {
        return encoding_;
    }

    void reset(c_size length, CodePoint* sharedHead) {
        my_destroy(sharedHead_, length_);
        my_delloc(sharedHead_);
        this->sharedHead_ = sharedHead;
        this->length_ = length;
    }

private:
    c_size length_;
    CodePoint* sharedHead_;
    Encoding* encoding_;
};

/** 
 * @brief 字符串 
 */
class String : public Sequence<String, CodePoint> {
    using self = String;
    using super = Sequence<self, CodePoint>;

    String(CodePoint* codePoints, c_size length, std::shared_ptr<StringManager> manager) :
            length_(length), codePoints_(codePoints), manager_(manager) {}

    String(c_size length, Encoding* encoding) :
            String(nullptr, length, std::make_shared<StringManager>(length, my_alloc<CodePoint>(length), encoding)) {
        for (c_size i = 0; i < length_; ++i) {
            my_construct(manager_->sharedHead() + i);
        }
        this->codePoints_ = manager_->sharedHead();
    }

public:
    String(const CString& encoding = UTF8) :
            String(0, encoding_map(encoding)) {}

    String(const char* str, c_size length = -1, const CString& encoding = UTF8) :
            String(0, encoding_map(encoding)) {
        length = ifelse(length > 0, length, std::strlen(str));
        auto [size, arr] = getCodePoints(str, length, manager_->encoding()).toArray().separate();
        this->length_ = size;
        this->codePoints_ = arr;
        this->manager_->reset(length_, codePoints_);
    }

    String(const CString& cstr, const CString& encoding = UTF8) :
            String(cstr.data(), cstr.size(), encoding) {}

    String(const self& other) :
            String(other.size(), other.encoding()) {
        this->length_ = size();
        for (c_size i = 0; i < length_; ++i) {
            at(i) = other.at(i);
        }
    }

    String(self&& other) noexcept :
            String(other.codePoints_, other.length_, other.manager_) {}

    ~String() = default;

    self& operator=(const self& other) {
        if (this == &other) return *this;

        my_destroy(&manager_);
        return *my_construct(this, other);
    }

    self& operator=(self&& other) noexcept {
        if (this == &other) return *this;

        my_destroy(&manager_);
        return *my_construct(this, std::move(other));
    }

    /**
     * @brief 字符串拼接
     */
    self operator+(const self& other) const {
        c_size m_size = this->size(), o_size = other.size();
        self res{m_size + o_size, this->encoding()};
        for (c_size i = 0; i < m_size; ++i) {
            res.at(i) = this->at(i);
        }
        for (c_size i = 0; i < o_size; ++i) {
            res.at(m_size + i) = other.at(i);
        }
        return res;
    }

    self& operator+=(const self& other) {
        self res = *this + other;
        *this = std::move(res);
        return *this;
    }

    /**
     * @brief 字符串复制n份
     */
    self operator*(c_size n) {
        c_size pos = 0, m_size = size();
        self res{m_size * n, encoding()};
        while (n--) {
            for (c_size i = 0; i < m_size; ++i) {
                res.at(pos++) = at(i);
            }
        }
        return res;
    }

    CodePoint& at(c_size index) {
        return codePoints_[index];
    }

    const CodePoint& at(c_size index) const {
        return codePoints_[index];
    }

    CodePoint& operator[](c_size index) {
        return at(index);
    }

    const CodePoint& operator[](c_size index) const {
        return at(index);
    }

    c_size size() const {
        return length_;
    }

    Encoding* encoding() const {
        return manager_->encoding();
    }

    /**
     * @brief 字符串的字节长度
     */
    c_size byteLength() const {
        c_size length = 0;
        for (auto&& ch : *this) {
            length += ch.size();
        }
        return length;
    }

    /** 
     * @brief 分片，[start, end)
     */
    self slice(c_size start, c_size end) {
        c_size m_size = size();
        start = neg_index(start, m_size);
        end = neg_index(end, m_size);
        return self{codePoints_ + start, end - start, manager_};
    }

    const self slice(c_size start, c_size end) const {
        c_size m_size = size();
        start = neg_index(start, m_size);
        end = neg_index(end, m_size);
        return self{codePoints_ + start, end - start, manager_};
    }

    /**
     * @brief 分片，[start, length)
     */
    self slice(c_size start) {
        return slice(start, size());
    }

    const self slice(c_size start) const {
        return slice(start, size());
    }

    /**
     * @brief 查找子串
     */
    c_size find(CodePoint c) const {
        return super::find(c);
    }

    c_size find(const self& pattern, c_size pos = 0) const {
        c_size m_size = size(), p_size = pattern.size();
        for (c_size i = pos; i + p_size <= m_size; ++i) {
            if (slice(i, i + p_size) == pattern) {
                return i;
            }
        }
        return -1;
    }

    Array<c_size> findAll(const self& pattern) const {
        DynArray<c_size> res;
        c_size pos = 0;
        while (true) {
            pos = find(pattern, pos);
            if (pos == -1) {
                break;
            }
            res.append(pos);
            pos += pattern.size();
        }
        return res.toArray();
    }

    bool startsWith(const self& prefix) const {
        if (size() < prefix.size()) {
            return false;
        }
        return slice(0, prefix.size()) == prefix;
    }

    bool endsWith(const self& suffix) const {
        if (size() < suffix.size()) {
            return false;
        }
        return slice(size() - suffix.size()) == suffix;
    }

    self upper() const {
        self res{*this};
        c_size m_size = size();
        for (c_size i = 0; i < m_size; ++i) {
            res.at(i) = res.at(i).upper();
        }
        return res;
    }

    self lower() const {
        self res{*this};
        c_size m_size = size();
        for (c_size i = 0; i < m_size; ++i) {
            res.at(i) = res.at(i).lower();
        }
        return res;
    }

    /**
     * @brief 去除字符串首尾空白字符
     */
    self trim() {
        auto [l, r] = get_trim_index();
        return slice(l, r);
    }

    const self trim() const {
        auto [l, r] = get_trim_index();
        return slice(l, r);
    }

    self ltrim() {
        return slice(get_ltrim_index());
    }

    const self ltrim() const {
        return slice(get_ltrim_index());
    }

    self rtrim() {
        return slice(get_rtrim_index());
    }

    const self rtrim() const {
        return slice(get_rtrim_index());
    }

    /**
     * @brief 去除字符串首尾模式串
     */
    self trim(const self& pattern) {
        auto [l, r] = get_trim_index(pattern);
        return slice(l, r);
    }

    const self trim(const self& pattern) const {
        auto [l, r] = get_trim_index(pattern);
        return slice(l, r);
    }

    self ltrim(const self& pattern) {
        return slice(get_ltrim_index(pattern));
    }

    const self ltrim(const self& pattern) const {
        return slice(get_ltrim_index(pattern));
    }

    self rtrim(const self& pattern) {
        return slice(get_rtrim_index(pattern));
    }

    const self rtrim(const self& pattern) const {
        return slice(get_rtrim_index(pattern));
    }

    /**
     * @brief 替换字符串中的子串
     */
    self replace(const self& old_, const self& new_) const {
        DynArray<c_size> indices;
        c_size pos = 0;
        while (pos = find(old_, pos), pos != -1) {
            indices.append(pos);
            pos += old_.size();
        }

        c_size m_size = size();
        self result{m_size + indices.size() * (new_.size() - old_.size()), encoding()};
        for (c_size i = 0, j = 0, k = 0; i < m_size; ++i) {
            if (j < indices.size() && i == indices[j]) {
                for (auto&& c : new_) {
                    result.codePoints_[k++] = c;
                }
                i += old_.size() - 1;
                ++j;
            } else {
                result.codePoints_[k++] = at(i);
            }
        }
        return result;
    }

    CString __str__() const {
        CString res{byteLength()};
        c_size pos = 0;
        for (auto&& ch : *this) {
            c_size ch_size = ch.size();
            std::memcpy(res.data() + pos, ch.data(), ch_size);
            pos += ch_size;
        }
        return res;
    }

    hash_t __hash__() const {
        return __str__().__hash__();
    }

private:
    std::pair<c_size, c_size> get_trim_index() const {
        c_size l = 0, r = size();
        while (l < r && at(l).isBlank()) ++l;
        while (l < r && at(r - 1).isBlank()) --r;
        return std::make_pair(l, r);
    }

    std::pair<c_size, c_size> get_trim_index(const self& pattern) const {
        c_size l = 0, r = size(), p_size = pattern.size();
        while (l + p_size <= r && slice(l, l + p_size) == pattern) l += p_size;
        while (l + p_size <= r && slice(r - p_size, r) == pattern) r -= p_size;
        return std::make_pair(l, r);
    }

    c_size get_ltrim_index() const {
        c_size l = 0, r = size();
        while (l < r && at(l).isBlank()) ++l;
        return l;
    }

    c_size get_ltrim_index(const self& pattern) const {
        c_size l = 0, r = size(), p_size = pattern.size();
        while (l + p_size <= r && slice(l, l + p_size) == pattern) l += p_size;
        return l;
    }

    c_size get_rtrim_index() const {
        c_size l = 0, r = size();
        while (l < r && at(r - 1).isBlank()) --r;
        return r;
    }

    c_size get_rtrim_index(const self& pattern) const {
        c_size l = 0, r = size(), p_size = pattern.size();
        while (l + p_size <= r && slice(r - p_size, r) == pattern) r -= p_size;
        return r;
    }

private:
    c_size length_;
    CodePoint* codePoints_;
    std::shared_ptr<StringManager> manager_;
};

def operator""_s(const char* str, size_t length)->String {
    return String{str, static_cast<c_size>(length)};
}

} // namespace my::util

#endif // STRING_HPP