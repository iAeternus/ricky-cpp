/**
 * @brief 字符串
 * @author Ricky
 * @date 2024/12/19
 * @version 1.0
 */
#ifndef STRING_HPP
#define STRING_HPP

#include <utility>

#include "CodePoint.hpp"
#include "NoCopy.hpp"

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
            length_(length), codePoints_(codePoints), manager_(std::move(manager)) {}

    String(c_size length, Encoding* encoding) :
            String(nullptr, length, std::make_shared<StringManager>(length, my_alloc<CodePoint>(length), encoding)) {
        for (c_size i = 0; i < length_; ++i) {
            my_construct(manager_->sharedHead() + i);
        }
        this->codePoints_ = manager_->sharedHead();
    }

public:
    constexpr static c_size npos = -1LL;

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
        c_size mSize = this->size(), oSize = other.size();
        self res{mSize + oSize, this->encoding()};
        for (c_size i = 0; i < mSize; ++i) {
            res.at(i) = this->at(i);
        }
        for (c_size i = 0; i < oSize; ++i) {
            res.at(mSize + i) = other.at(i);
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
        c_size pos = 0, mSize = size();
        self res{mSize * n, encoding()};
        while (n--) {
            for (c_size i = 0; i < mSize; ++i) {
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
        c_size length = 0LL;
        for (auto&& ch : *this) {
            length += ch.size();
        }
        return length;
    }

    /**
     * @brief 分割字符串，[start, end)
     */
    self split(c_size start, c_size end) {
        c_size mSize = size();
        start = neg_index(start, mSize);
        end = neg_index(end, mSize);
        return self{codePoints_ + start, end - start, manager_};
    }

    const self split(c_size start, c_size end) const {
        c_size mSize = size();
        start = neg_index(start, mSize);
        end = neg_index(end, mSize);
        return self{codePoints_ + start, end - start, manager_};
    }

    /**
     * @brief 分割字符串，[start, length)
     */
    self split(c_size start) {
        return split(start, size());
    }

    const self split(c_size start) const {
        return split(start, size());
    }

    /**
     * @brief 查找子串
     */
    c_size find(const CodePoint& c) const {
        return super::find(c);
    }

    c_size find(const self& pattern, c_size pos = 0) const {
        c_size mSize = size(), p_size = pattern.size();
        for (c_size i = pos; i + p_size <= mSize; ++i) {
            if (split(i, i + p_size) == pattern) {
                return i;
            }
        }
        return npos;
    }

    Array<c_size> findAll(const self& pattern) const {
        DynArray<c_size> res;
        c_size pos = 0LL;
        while (true) {
            pos = find(pattern, pos);
            if (pos == npos) {
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
        return split(0, prefix.size()) == prefix;
    }

    bool endsWith(const self& suffix) const {
        if (size() < suffix.size()) {
            return false;
        }
        return split(size() - suffix.size()) == suffix;
    }

    self upper() const {
        self res{*this};
        c_size mSize = size();
        for (c_size i = 0; i < mSize; ++i) {
            res.at(i) = res.at(i).upper();
        }
        return res;
    }

    self lower() const {
        self res{*this};
        c_size mSize = size();
        for (c_size i = 0; i < mSize; ++i) {
            res.at(i) = res.at(i).lower();
        }
        return res;
    }

    /**
     * @brief 去除字符串首尾空白字符
     */
    self trim() {
        auto [l, r] = get_trim_index();
        return split(l, r);
    }

    const self trim() const {
        auto [l, r] = get_trim_index();
        return split(l, r);
    }

    self ltrim() {
        return split(get_ltrim_index());
    }

    const self ltrim() const {
        return split(get_ltrim_index());
    }

    self rtrim() {
        return split(get_rtrim_index());
    }

    const self rtrim() const {
        return split(get_rtrim_index());
    }

    /**
     * @brief 去除字符串首尾模式串
     */
    self trim(const self& pattern) {
        auto [l, r] = get_trim_index(pattern);
        return split(l, r);
    }

    const self trim(const self& pattern) const {
        auto [l, r] = get_trim_index(pattern);
        return split(l, r);
    }

    self ltrim(const self& pattern) {
        return split(get_ltrim_index(pattern));
    }

    const self ltrim(const self& pattern) const {
        return split(get_ltrim_index(pattern));
    }

    self rtrim(const self& pattern) {
        return split(get_rtrim_index(pattern));
    }

    const self rtrim(const self& pattern) const {
        return split(get_rtrim_index(pattern));
    }

    // /**
    //  * @brief 使用this连接可迭代对象的每个元素
    //  * @param iter 可迭代对象
    //  * @return 返回构建好的结果对象
    //  */
    // template <Iterable I>
    // self join(const I& iter) const {
    //     c_size newLength = 0LL, pos = 0LL, mSize = this->size();
    //     for (auto&& elem : iter) {
    //         newLength = elem.size() + mSize;
    //     }
    //     newLength = std::max<c_size>(0LL, newLength - mSize);

    //     self result{newLength, encoding()};
    //     for (auto&& elem : iter) {
    //         for (c_size i = 0, eSize = elem.size(); i < eSize; ++i) {
    //             result.at(pos++) = elem.at(i);
    //         }
    //         if (pos < result.size()) {
    //             for (c_size i = 0; i < mSize; ++i) {
    //                 result.at(pos++) = this->at(i);
    //             }
    //         }
    //     }
    //     return result;
    // }

    /**
     * @brief 替换字符串中的子串
     */
    self replace(const self& old_, const self& new_) const {
        DynArray<c_size> indices;
        c_size pos = 0LL;
        while (pos = find(old_, pos), pos != npos) {
            indices.append(pos);
            pos += old_.size();
        }

        c_size mSize = size();
        self result{mSize + indices.size() * (new_.size() - old_.size()), encoding()};
        for (c_size i = 0, j = 0, k = 0; i < mSize; ++i) {
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

    /**
     * @brief 查找第一个成对出现的字符
     * @return 返回从left到right的子字符串
     */
    self match(const CodePoint& left, const CodePoint& right) const {
        c_size l = find(left);
        if (l == npos) {
            return "";
        }

        c_size matchCount = 1;
        for (c_size r = l + 1, mSize = size(); r < mSize; ++r) {
            if (at(r) == right) {
                --matchCount;
            } else if (at(r) == left) {
                ++matchCount;
            }

            if (matchCount == 0) {
                return split(l, r + 1);
            }
        }
        ValueError("Unmatched parentheses, too many left parentheses");
        return None<String>;
    }

    /**
     * 删除字符串中所有指定字符
     */
    self removeAll(CodePoint&& codePoint) const {
        c_size mSize = size();
        std::stringstream stream;
        for (c_size i = 0; i < mSize; ++i) {
            if (at(i) != codePoint) {
                stream << at(i);
            }
        }
        return self{stream.str().data()};
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
        while (l + p_size <= r && split(l, l + p_size) == pattern) l += p_size;
        while (l + p_size <= r && split(r - p_size, r) == pattern) r -= p_size;
        return std::make_pair(l, r);
    }

    c_size get_ltrim_index() const {
        c_size l = 0, r = size();
        while (l < r && at(l).isBlank()) ++l;
        return l;
    }

    c_size get_ltrim_index(const self& pattern) const {
        c_size l = 0, r = size(), p_size = pattern.size();
        while (l + p_size <= r && split(l, l + p_size) == pattern) l += p_size;
        return l;
    }

    c_size get_rtrim_index() const {
        c_size l = 0, r = size();
        while (l < r && at(r - 1).isBlank()) --r;
        return r;
    }

    c_size get_rtrim_index(const self& pattern) const {
        c_size l = 0, r = size(), p_size = pattern.size();
        while (l + p_size <= r && split(r - p_size, r) == pattern) r -= p_size;
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