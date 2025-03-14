/**
 * @brief 自定义字符串类，支持 Unicode 编码
 * @author Ricky
 * @date 2024/12/19
 * @version 1.0
 */
#ifndef STRING_HPP
#define STRING_HPP

#include <utility>

#include "CodePoint.hpp"
#include "NoCopy.hpp"
#include "Vector.hpp"

namespace my::util {

/**
 * @brief 字符串管理器，用于管理字符串的共享数据和编码信息
 */
class StringManager : public Object<StringManager>, public NoCopy {
public:
    using self = StringManager;

    /**
     * @brief 构造函数
     * @param length 字符串的长度
     * @param sharedHead 共享的码点数组
     * @param encoding 字符串的编码
     */
    StringManager(isize length, util::CodePoint* sharedHead, util::Encoding* encoding) :
            length_(length), sharedHead_(sharedHead), encoding_(encoding) {}

    /**
     * @brief 析构函数，释放共享的码点数组
     */
    ~StringManager() {
        my_destroy(sharedHead_, length_);
        my_delloc(sharedHead_);
        length_ = 0;
    }

    /**
     * @brief 获取共享的码点数组
     * @return 共享的码点数组
     */
    util::CodePoint* sharedHead() const {
        return sharedHead_;
    }

    /**
     * @brief 获取字符串的编码
     * @return 字符串的编码
     */
    util::Encoding* encoding() const {
        return encoding_;
    }

    /**
     * @brief 重置字符串管理器
     * @param length 新的字符串长度
     * @param sharedHead 新的共享码点数组
     */
    void reset(isize length, util::CodePoint* sharedHead) {
        my_destroy(sharedHead_, length_);
        my_delloc(sharedHead_);
        this->sharedHead_ = sharedHead;
        this->length_ = length;
    }

private:
    isize length_;                // 字符串的长度
    util::CodePoint* sharedHead_; // 共享的码点数组
    util::Encoding* encoding_;    // 字符串的编码
};

/**
 * @brief 字符串，支持 Unicode 编码和多种操作
 */
class String : public util::Sequence<String, util::CodePoint> {
    friend class StringBuilder;

    /**
     * @brief 内部构造函数，用于创建字符串对象
     * @param codePoints 字符串的码点数组
     * @param length 字符串的长度
     * @param manager 字符串管理器
     */
    String(util::CodePoint* codePoints, isize length, std::shared_ptr<StringManager> manager) :
            length_(length), codePoints_(codePoints), manager_(std::move(manager)) {}

    /**
     * @brief 内部构造函数，用于根据编码创建字符串对象
     * @param length 字符串的长度
     * @param encoding 字符串的编码
     */
    String(isize length, util::Encoding* encoding) :
            String(nullptr, length, std::make_shared<StringManager>(length, my_alloc<util::CodePoint>(length), encoding)) {
        for (isize i = 0; i < length_; ++i) {
            my_construct(manager_->sharedHead() + i);
        }
        this->codePoints_ = manager_->sharedHead();
    }

public:
    using self = String;
    using super = util::Sequence<self, util::CodePoint>;
    static constexpr isize npos = -1LL; // 无效的索引位置

    /**
     * @brief 默认构造函数，创建一个空字符串，并指定编码
     * @param encoding 字符串的编码（可选）
     */
    String(const CString& encoding = util::UTF8) :
            String(0, util::encoding_map(encoding)) {}

    /**
     * @brief 构造函数，根据 C 风格字符串创建字符串对象
     * @param str C 风格字符串
     * @param length 字符串的长度（可选）
     * @param encoding 字符串的编码（可选）
     */
    String(const char* str, isize length = -1, const CString& encoding = util::UTF8) :
            String(0, util::encoding_map(encoding)) {
        length = ifelse(length > 0, length, std::strlen(str));
        auto [size, arr] = getCodePoints(str, length, manager_->encoding()).separate();
        this->length_ = size;
        this->codePoints_ = arr;
        this->manager_->reset(length_, codePoints_);
    }

    /**
     * @brief 构造函数，根据自定义字符串创建字符串对象
     * @param cstr 自定义字符串
     * @param encoding 字符串的编码（可选）
     */
    String(const CString& cstr, const CString& encoding = util::UTF8) :
            String(cstr.data(), cstr.size(), encoding) {}

    /**
     * @brief 拷贝构造函数
     * @param other 要拷贝的字符串对象
     */
    String(const self& other) :
            String(other.size(), other.encoding()) {
        for (isize i = 0; i < length_; ++i) {
            at(i) = other.at(i);
        }
    }

    /**
     * @brief 移动构造函数
     * @param other 要移动的字符串对象
     */
    String(self&& other) noexcept :
            String(other.codePoints_, other.length_, other.manager_) {}

    /**
     * @brief 默认析构函数
     */
    ~String() = default;

    /**
     * @brief 拷贝赋值操作符
     * @param other 要赋值的字符串对象
     * @return 自身的引用
     */
    self& operator=(const self& other) {
        if (this == &other) return *this;

        my_destroy(&manager_);
        return *my_construct(this, other);
    }

    /**
     * @brief 移动赋值操作符
     * @param other 要赋值的字符串对象
     * @return 自身的引用
     */
    self& operator=(self&& other) noexcept {
        if (this == &other) return *this;

        my_destroy(&manager_);
        return *my_construct(this, std::move(other));
    }

    /**
     * @brief 字符串拼接操作符
     * @param other 要拼接的字符串对象
     * @return 拼接后的新字符串
     */
    self operator+(const self& other) const {
        isize mSize = this->size(), oSize = other.size();
        self res{mSize + oSize, this->encoding()};
        for (isize i = 0; i < mSize; ++i) {
            res.at(i) = this->at(i);
        }
        for (isize i = 0; i < oSize; ++i) {
            res.at(mSize + i) = other.at(i);
        }
        return res;
    }

    /**
     * @brief 累加字符串拼接操作符
     * @param other 要拼接的字符串对象
     * @return 自身的引用
     */
    self& operator+=(const self& other) {
        self res = *this + other;
        *this = std::move(res);
        return *this;
    }

    /**
     * @brief 字符串复制操作符
     * @param n 复制的份数
     * @return 复制后的新字符串
     */
    self operator*(isize n) {
        isize pos = 0, mSize = size();
        self res{mSize * n, encoding()};
        while (n--) {
            for (isize i = 0; i < mSize; ++i) {
                res.at(pos++) = at(i);
            }
        }
        return res;
    }

    /**
     * @brief 获取指定索引处的码点
     * @param index 索引位置
     * @return 索引位置的码点
     */
    util::CodePoint& at(isize index) {
        return codePoints_[index];
    }

    /**
     * @brief 获取指定索引处的码点（常量版本）
     * @param index 索引位置
     * @return 索引位置的码点
     */
    const util::CodePoint& at(isize index) const {
        return codePoints_[index];
    }

    /**
     * @brief 索引操作符，返回指定索引处的码点
     * @param index 索引位置
     * @return 索引位置的码点
     */
    util::CodePoint& operator[](isize index) {
        return at(index);
    }

    /**
     * @brief 索引操作符（常量版本），返回指定索引处的码点
     * @param index 索引位置
     * @return 索引位置的码点
     */
    const util::CodePoint& operator[](isize index) const {
        return at(index);
    }

    /**
     * @brief 获取字符串的长度
     * @return 字符串的长度
     */
    isize size() const {
        return length_;
    }

    /**
     * @brief 获取字符串的编码
     * @return 字符串的编码
     */
    util::Encoding* encoding() const {
        return manager_->encoding();
    }

    /**
     * @brief 获取字符串的字节长度
     * @return 字符串的字节长度
     */
    isize byteLength() const {
        isize length = 0;
        for (auto&& ch : *this) {
            length += ch.size();
        }
        return length;
    }

    /**
     * @brief 分割字符串，返回指定范围的子字符串
     * @param start 起始索引
     * @param end 结束索引（不包含）
     * @return 子字符串
     */
    self split(isize start, isize end) {
        isize mSize = size();
        start = neg_index(start, mSize);
        end = neg_index(end, mSize);
        return self{codePoints_ + start, end - start, manager_};
    }

    /**
     * @brief 分割字符串，返回指定范围的子字符串（常量版本）
     * @param start 起始索引
     * @param end 结束索引（不包含）
     * @return 子字符串
     */
    const self split(isize start, isize end) const {
        isize mSize = size();
        start = neg_index(start, mSize);
        end = neg_index(end, mSize);
        return self{codePoints_ + start, end - start, manager_};
    }

    /**
     * @brief 分割字符串，返回从指定索引开始到末尾的子字符串
     * @param start 起始索引
     * @return 子字符串
     */
    self split(isize start) {
        return split(start, size());
    }

    /**
     * @brief 分割字符串，返回从指定索引开始到末尾的子字符串（常量版本）
     * @param start 起始索引
     * @return 子字符串
     */
    const self split(isize start) const {
        return split(start, size());
    }

    /**
     * @brief 查找字符在字符串中的第一个位置
     * @param c 要查找的字符
     * @return 字符的位置，未找到返回 `npos`
     */
    isize find(const util::CodePoint& c) const {
        return super::find(c);
    }

    /**
     * @brief 查找模式串的第一个匹配位置
     * @param pattern 模式串，长度为m
     * @param pos 起始查找位置（可选）
     * @return 模式串的第一个匹配位置，未找到返回 `npos`
     * @note KMP算法，时间复杂度 O(n + m)，n为文本串的长度
     */
    isize find(const self& pattern, isize pos = 0) const {
        isize mSize = size(), pSize = pattern.size();
        auto next = get_next(pattern);
        for (isize i = pos, j = 0; i < mSize; ++i) {
            // 失配，j按照next回跳
            while (j > 0 && at(i) != pattern[j]) {
                j = next[j - 1];
            }
            // 匹配，j前进
            if (at(i) == pattern[j]) {
                ++j;
            }
            // 模式串匹配完，返回文本串匹配起点
            if (j == pSize) {
                return i - pSize + 1;
            }
        }
        return npos;
    }

    /**
     * @brief 查找模式串的所有匹配位置
     * @param pattern 模式串，长度为m
     * @return 所有匹配位置
     * @note KMP算法，时间复杂度 O(n + m)，n为文本串的长度
     */
    util::Vector<isize> findAll(const self& pattern) const {
        util::Vector<isize> res;
        isize mSize = size(), pSize = pattern.size();
        auto next = get_next(pattern);
        for (isize i = 0, j = 0; i < mSize; ++i) {
            // 失配，j按照next回跳
            while (j > 0 && at(i) != pattern[j]) {
                j = next[j - 1];
            }
            // 匹配，j前进
            if (at(i) == pattern[j]) {
                ++j;
            }
            // 模式串匹配完，收集文本串匹配起点
            if (j == pSize) {
                res.append(i - pSize + 1);
                j = next[j - 1];
            }
        }
        return res;
    }

    /**
     * @brief 检查字符串是否以指定子字符串开头
     * @param prefix 要检查的子字符串
     * @return 是否以指定子字符串开头
     */
    bool startsWith(const self& prefix) const {
        if (size() < prefix.size()) {
            return false;
        }
        return split(0, prefix.size()) == prefix;
    }

    /**
     * @brief 检查字符串是否以指定子字符串结尾
     * @param suffix 要检查的子字符串
     * @return 是否以指定子字符串结尾
     */
    bool endsWith(const self& suffix) const {
        if (size() < suffix.size()) {
            return false;
        }
        return split(size() - suffix.size()) == suffix;
    }

    /**
     * @brief 将字符串转换为全大写
     * @return 全大写的字符串
     */
    self upper() const {
        self res{*this};
        isize mSize = size();
        for (isize i = 0; i < mSize; ++i) {
            res.at(i) = res.at(i).upper();
        }
        return res;
    }

    /**
     * @brief 将字符串转换为全小写
     * @return 全小写的字符串
     */
    self lower() const {
        self res{*this};
        isize mSize = size();
        for (isize i = 0; i < mSize; ++i) {
            res.at(i) = res.at(i).lower();
        }
        return res;
    }

    /**
     * @brief 去除字符串首尾的空白字符
     * @return 去除空白后的字符串
     */
    self trim() {
        auto [l, r] = get_trim_index();
        return split(l, r);
    }

    /**
     * @brief 去除字符串首尾的空白字符（常量版本）
     * @return 去除空白后的字符串
     */
    const self trim() const {
        auto [l, r] = get_trim_index();
        return split(l, r);
    }

    /**
     * @brief 去除字符串首部的空白字符
     * @return 去除首部空白后的字符串
     */
    self ltrim() {
        return split(get_ltrim_index());
    }

    /**
     * @brief 去除字符串首部的空白字符（常量版本）
     * @return 去除首部空白后的字符串
     */
    const self ltrim() const {
        return split(get_ltrim_index());
    }

    /**
     * @brief 去除字符串尾部的空白字符
     * @return 去除尾部空白后的字符串
     */
    self rtrim() {
        return split(get_rtrim_index());
    }

    /**
     * @brief 去除字符串尾部的空白字符（常量版本）
     * @return 去除尾部空白后的字符串
     */
    const self rtrim() const {
        return split(get_rtrim_index());
    }

    /**
     * @brief 去除字符串首尾的指定模式
     * @param pattern 要去除的模式
     * @return 去除模式后的字符串
     */
    self trim(const self& pattern) {
        auto [l, r] = get_trim_index(pattern);
        return split(l, r);
    }

    /**
     * @brief 去除字符串首尾的指定模式（常量版本）
     * @param pattern 要去除的模式
     * @return 去除模式后的字符串
     */
    const self trim(const self& pattern) const {
        auto [l, r] = get_trim_index(pattern);
        return split(l, r);
    }

    /**
     * @brief 去除字符串首部的指定模式
     * @param pattern 要去除的模式
     * @return 去除模式后的字符串
     */
    self ltrim(const self& pattern) {
        return split(get_ltrim_index(pattern));
    }

    /**
     * @brief 去除字符串首部的指定模式（常量版本）
     * @param pattern 要去除的模式
     * @return 去除模式后的字符串
     */
    const self ltrim(const self& pattern) const {
        return split(get_ltrim_index(pattern));
    }

    /**
     * @brief 去除字符串尾部的指定模式
     * @param pattern 要去除的模式
     * @return 去除模式后的字符串
     */
    self rtrim(const self& pattern) {
        return split(get_rtrim_index(pattern));
    }

    /**
     * @brief 去除字符串尾部的指定模式（常量版本）
     * @param pattern 要去除的模式
     * @return 去除模式后的字符串
     */
    const self rtrim(const self& pattern) const {
        return split(get_rtrim_index(pattern));
    }

    // /**
    //  * @brief 使用当前字符串连接可迭代对象的每个元素
    //  * @param iter 可迭代对象
    //  * @return 连接后的字符串
    //  */
    // template <Iterable I>
    // self join(const I& iter) const {
    //     isize newLength = 0LL, pos = 0LL;
    //     for (auto&& elem : iter) {
    //         newLength = elem.size() + length_;
    //     }
    //     newLength = math::max<isize>(0LL, newLength - length_);

    //     self result{newLength, encoding()};
    //     for (auto&& elem : iter) {
    //         for (isize i = 0, eSize = elem.size(); i < eSize; ++i) {
    //             result.at(pos++) = elem.at(i);
    //         }
    //         if (pos < result.size()) {
    //             for (isize i = 0; i < length_; ++i) {
    //                 result.at(pos++) = this->at(i);
    //             }
    //         }
    //     }
    //     return result;
    // }

    /**
     * @brief 替换字符串中的子字符串
     * @param old_ 要被替换的子字符串
     * @param new_ 替换的新字符串
     * @return 替换后的新字符串
     */
    self replace(const self& old_, const self& new_) const {
        auto indices = findAll(old_);

        isize mSize = size();
        self result{mSize + indices.size() * (new_.size() - old_.size()), encoding()};
        for (isize i = 0, j = 0, k = 0; i < mSize; ++i) {
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
     * @brief 查找第一个成对出现的字符，并返回子字符串
     * @param left 左字符
     * @param right 右字符
     * @return 包含两个字符的子字符串
     */
    self match(const util::CodePoint& left, const util::CodePoint& right) const {
        isize l = find(left);
        if (l == npos) {
            return "";
        }

        isize matchCount = 1;
        for (isize r = l + 1, mSize = size(); r < mSize; ++r) {
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
     * @brief 删除字符串中所有指定字符
     * @param codePoint 要删除的字符
     * @return 删除后的字符串
     */
    self removeAll(util::CodePoint&& codePoint) const {
        isize mSize = size();
        std::stringstream stream;
        for (isize i = 0; i < mSize; ++i) {
            if (at(i) != codePoint) {
                stream << at(i);
            }
        }
        return self{stream.str().data()};
    }

    /**
     * @brief 返回字符串的 C 风格字符串表示
     * @return C 风格字符串
     */
    CString __str__() const {
        CString res{byteLength()};
        isize pos = 0;
        for (auto&& ch : *this) {
            isize ch_size = ch.size();
            std::memcpy(res.data() + pos, ch.data(), ch_size);
            pos += ch_size;
        }
        return res;
    }

    /**
     * @brief 返回字符串的哈希值
     * @return 字符串的哈希值
     */
    hash_t __hash__() const {
        return __str__().__hash__();
    }

    cmp_t __cmp__(const self& other) const {
        isize minSize = math::min(this->size(), other.size());
        for (auto i = 0; i < minSize; ++i) {
            auto cmp = this->at(i).__cmp__(other.at(i));
            if (cmp != 0) {
                return cmp;
            }
        }
        return static_cast<isize>(this->size() - other.size());
    }

private:
    /**
     * @brief 获取去除首尾空白后的索引范围
     * @return 首尾空白后的索引范围
     */
    std::pair<isize, isize> get_trim_index() const {
        isize l = 0, r = size();
        while (l < r && at(l).isBlank()) ++l;
        while (l < r && at(r - 1).isBlank()) --r;
        return std::make_pair(l, r);
    }

    /**
     * @brief 获取去除首尾模式后的索引范围
     * @param pattern 要去除的模式
     * @return 首尾模式后的索引范围
     */
    std::pair<isize, isize> get_trim_index(const self& pattern) const {
        isize l = 0, r = size(), p_size = pattern.size();
        while (l + p_size <= r && split(l, l + p_size) == pattern) l += p_size;
        while (l + p_size <= r && split(r - p_size, r) == pattern) r -= p_size;
        return std::make_pair(l, r);
    }

    /**
     * @brief 获取去除首部空白后的索引
     * @return 去除首部空白后的索引
     */
    isize get_ltrim_index() const {
        isize l = 0, r = size();
        while (l < r && at(l).isBlank()) ++l;
        return l;
    }

    /**
     * @brief 获取去除首部模式后的索引
     * @param pattern 要去除的模式
     * @return 去除首部模式后的索引
     */
    isize get_ltrim_index(const self& pattern) const {
        isize l = 0, r = size(), p_size = pattern.size();
        while (l + p_size <= r && split(l, l + p_size) == pattern) l += p_size;
        return l;
    }

    /**
     * @brief 获取去除尾部空白后的索引
     * @return 去除尾部空白后的索引
     */
    isize get_rtrim_index() const {
        isize l = 0, r = size();
        while (l < r && at(r - 1).isBlank()) --r;
        return r;
    }

    /**
     * @brief 获取去除尾部模式后的索引
     * @param pattern 要去除的模式
     * @return 去除尾部模式后的索引
     */
    isize get_rtrim_index(const self& pattern) const {
        isize l = 0, r = size(), p_size = pattern.size();
        while (l + p_size <= r && split(r - p_size, r) == pattern) r -= p_size;
        return r;
    }

private:
    isize length_;                           // 字符串的长度
    util::CodePoint* codePoints_;            // 字符串的码点数组
    std::shared_ptr<StringManager> manager_; // 字符串管理器
private:
    /**
     * @brief KMP辅助函数，求next数组
     * @param pattern 模式串
     * @note next[i]: 模式串[0, i)中最长相等前后缀的长度为next[i]
     * @note 时间复杂度为 O(m)，m为模式串的长度
     */
    static util::Vector<isize> get_next(const self& pattern) {
        auto pSize = pattern.size();
        util::Vector<isize> next(pSize, 0);
        for (isize i = 1, j = 0; i < pSize; ++i) {
            // 失配，j按照next数组回跳
            while (j > 0 && pattern[i] != pattern[j]) {
                j = next[j - 1];
            }
            // 匹配，j前进
            if (pattern[i] == pattern[j]) {
                ++j;
            }
            next[i] = j;
        }
        return next;
    }
};

/**
 * @brief 字符串构建器
 * @class StringBuilder
 */
class StringBuilder : public Object<StringBuilder> {
public:
    using self = StringBuilder;

    /**
     * @brief 默认构造函数
     */
    explicit StringBuilder(util::Encoding* enc = util::encoding_map(util::UTF8)) :
            encoding_(enc) {}

    /**
     * @brief 追加字符串
     * @note 允许链式调用
     */
    self& append(const String& str) {
        if (str.encoding() != encoding_) {
            EncodingError("Encoding mismatch");
        }
        for (isize i = 0; i < str.size(); ++i) {
            buffer_.append(str.at(i));
        }
        return *this;
    }

    /**
     * @brief 追加 C 字符串
     * @note 允许链式调用
     */
    self& append(const char* cstr) {
        isize len = std::strlen(cstr);
        isize i = 0;
        while (i < len) {
            CodePoint cp(cstr + i, encoding_);
            i += cp.size();
            buffer_.append(std::move(cp)); // 移动语义
        }
        return *this;
    }

    /**
     * @brief 生成 String
     * @note 内部实现对数据的转移
     */
    String str() {
        auto length = buffer_.size();
        auto [size, codePoints] = buffer_.separate();
        return String(codePoints, length, std::make_shared<StringManager>(length, codePoints, encoding_));
    }

    /**
     * @brief 清空构造器
     */
    void clear() {
        buffer_.clear();
    }

private:
    util::Vector<util::CodePoint> buffer_; // 缓冲区
    util::Encoding* encoding_;             // 字符串编码
};

} // namespace my::util

namespace my {

/**
 * @brief 自定义字符串字面量，支持 `_s` 后缀转换为 `String` 对象
 * @param str C 风格字符串
 * @param length 字符串长度
 * @return 转换后的 `String` 对象
 */
fn operator""_s(const char* str, size_t length)->const util::String {
    return util::String{str, static_cast<isize>(length)};
}

} // namespace my

#endif // STRING_HPP