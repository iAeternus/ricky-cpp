/**
 * @brief 字符串视图
 * @author Ricky
 * @date 2025/8/19
 * @version 1.0
 */

#ifndef STRING_VIEW_HPP
#define STRING_VIEW_HPP

#include "CodePoint.hpp"
#include "Encoding.hpp"

namespace my::util {

template <typename Alloc>
class BasicString;

/**
 * @class BasicStringView
 * @brief 字符串视图只读视图
 * @note 源字符串生命周期必须不短于视图生命周期
 * @tparam Iter 迭代器类型
 */
template <typename Iter, typename Alloc = Allocator<CodePoint>>
class BasicStringView : public Object<BasicStringView<Iter>> {
public:
    using Self = BasicStringView<Iter, Alloc>;
    using const_iterator = Iter;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    constexpr BasicStringView() noexcept = default;

    // /**
    //  * @breif 通过字符指针和长度构造
    //  * @param str 字符指针
    //  * @param size 长度
    //  */
    // BasicStringView(const char* str, const usize size) noexcept :
    //         begin_(Iter{str}), end_(Iter{str + size}), encoding_(encoding_map(EncodingType::ASCII)) {}
    //
    // /**
    //  * @breif 通过字符指针构造
    //  * @param str 字符指针
    //  */
    // BasicStringView(const char* str) noexcept :
    //         Self(str, str + std::strlen(str)) {}
    //
    // BasicStringView(const char* begin, const char* end) noexcept :
    //         begin_(Iter{begin}), end_(Iter{end}), encoding_(encoding_map(EncodingType::ASCII)) {}

    /**
     * @brief 通过首尾迭代器构造
     * @param begin 视图起始迭代器
     * @param end 视图尾后迭代器
     * @param enc 字符串编码
     */
    BasicStringView(Iter begin, Iter end, const EncodingType enc = EncodingType::UTF8) noexcept :
            begin_(begin), end_(end), encoding_(encoding_map(enc)) {}

    /**
     * @brief 通过字符串对象构造全视图
     */
    BasicStringView(const BasicString<Alloc>& str) noexcept :
            begin_(str.cbegin()), end_(str.cend()), encoding_(str.encoding()) {}

    /**
     * @brief 通过绝对首索引和视图长度构造
     */
    BasicStringView(const BasicString<Alloc>& str, usize pos, usize len) noexcept :
            encoding_(str.encoding()) {
        const auto size = str.length();
        pos = std::min(pos, size);
        len = std::min(len, size - pos);

        begin_ = str.cbegin() + pos;
        end_ = begin_ + len;
    }

    /**
     * @brief 视图长度（码点数），适配可迭代约束
     * @return 视图长度（码点数）
     */
    constexpr usize size() const noexcept {
        return static_cast<usize>(std::distance(begin_, end_));
    }

    /**
     * @brief 视图长度（码点数）
     */
    constexpr usize length() const noexcept {
        return static_cast<usize>(std::distance(begin_, end_));
    }

    /**
     * @brief 字符串视图是否为空
     * @return true=是 false=否
     */
    constexpr bool empty() const noexcept {
        return length() == 0;
    }

    /**
     * @brief 获取编码
     * @return 编码
     */
    constexpr Encoding* encoding() const noexcept {
        return encoding_;
    }

    /**
     * @brief 索引访问，无边界检查
     */
    decltype(auto) operator[](usize idx) const noexcept {
        return *(begin_ + idx);
    }

    /**
     * @brief 字符串切片，返回指定范围的子字符串
     * @param start 起始索引
     * @param end 结束索引（不包含）
     * @return 子字符串
     */
    Self slice(usize start, isize end) const {
        end = neg_index(end, static_cast<isize>(length()));
        return Self(begin() + start, begin() + static_cast<usize>(end), encoding_->type());
    }

    /**
     * @brief 字符串切片，返回从指定索引开始到末尾的子字符串
     * @param start 起始索引
     * @return 子字符串
     */
    Self slice(const usize start) const {
        return slice(start, size());
    }

    /**
     * @brief 查找字符在字符串中的第一个位置
     * @param c 要查找的字符
     * @return 字符的位置，未找到返回 `npos`
     */
    usize find(const CodePoint& c) const {
        usize idx = 0;
        for (auto it = begin(); it != end(); ++it) {
            if (*it == c) {
                return idx;
            }
            ++idx;
        }
        return npos;
    }

    /**
     * @brief 查找模式串的第一个匹配位置
     * @param pattern 模式串，长度为m
     * @param pos 起始查找位置（可选）
     * @return 模式串的第一个匹配位置，未找到返回 `npos`
     * @note KMP算法，时间复杂度 O(n + m)，n为文本串的长度
     */
    usize find(const Self pattern, const usize pos = 0) const {
        if (pattern.empty()) return npos;
        const auto m_size = length(), p_size = pattern.length();
        const auto next = get_next(pattern);
        for (usize i = pos, j = 0; i < m_size; ++i) {
            // 失配，j按照next回跳
            while (j > 0 && this->operator[](i) != pattern[j]) {
                j = next[j - 1];
            }
            j += this->operator[](i) == pattern[j]; // 匹配，j前进
            // 模式串匹配完，返回文本串匹配起点
            if (j == p_size) {
                return i - p_size + 1;
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
    Vec<usize> find_all(const Self pattern) const {
        Vec<usize> res;
        if (pattern.empty()) return res;
        const auto m_size = length(), p_size = pattern.length();
        const auto next = get_next(pattern);
        for (usize i = 0, j = 0; i < m_size; ++i) {
            // 失配，j按照next回跳
            while (j > 0 && this->operator[](i) != pattern[j]) {
                j = next[j - 1];
            }
            j += this->operator[](i) == pattern[j]; // 匹配，j前进
            // 模式串匹配完，收集文本串匹配起点
            if (j == p_size) {
                res.append(i - p_size + 1);
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
    bool starts_with(const Self prefix) const {
        if (length() < prefix.size()) {
            return false;
        }
        return slice(0, prefix.size()) == prefix;
    }

    /**
     * @brief 检查字符串是否以指定子字符串结尾
     * @param suffix 要检查的子字符串
     * @return 是否以指定子字符串结尾
     */
    bool ends_with(const Self suffix) const {
        if (length() < suffix.size()) {
            return false;
        }
        return slice(length() - suffix.size()) == suffix;
    }

    /**
     * @brief 去除字符串首尾的空白字符
     * @return 去除空白后的字符串
     */
    Self trim() const {
        auto [l, r] = get_trim_index();
        return slice(l, r);
    }

    /**
     * @brief 去除字符串首部的空白字符
     * @return 去除首部空白后的字符串
     */
    Self ltrim() const {
        return slice(get_ltrim_index());
    }

    /**
     * @brief 去除字符串尾部的空白字符
     * @return 去除尾部空白后的字符串
     */
    Self rtrim() const {
        return slice(get_rtrim_index());
    }

    /**
     * @brief 去除字符串首尾的指定模式
     * @param pattern 要去除的模式
     * @return 去除模式后的字符串
     */
    Self trim(const Self pattern) const {
        auto [l, r] = get_trim_index(pattern);
        return slice(l, r);
    }

    /**
     * @brief 去除字符串首部的指定模式
     * @param pattern 要去除的模式
     * @return 去除模式后的字符串
     */
    Self ltrim(const Self pattern) const {
        return slice(get_ltrim_index(pattern));
    }

    /**
     * @brief 去除字符串尾部的指定模式
     * @param pattern 要去除的模式
     * @return 去除模式后的字符串
     */
    Self rtrim(const Self pattern) const {
        return slice(get_rtrim_index(pattern));
    }

    /**
     * @brief 转换为完整字符串，会拷贝数据
     */
    constexpr BasicString<Alloc> to_string() const {
        return BasicString<Alloc>{begin_, end_, encoding_};
    }

    [[nodiscard]] bool __equals__(const Self& other) const {
        if (length() != other.length()) {
            return false;
        }
        if (begin_ == other.begin_) {
            return true;
        }
        return std::equal(begin_, end_, other.begin_);
    }

    // TODO 段错误
    // [[nodiscard]] cmp_t __cmp__(const Self& other) const {
    //     auto min_size = std::min(this->length(), other.length());
    //     for (usize i = 0; i < min_size; ++i) {
    //         auto cmp = this->operator[](i).__cmp__(other[i]);
    //         if (cmp != 0) {
    //             return cmp;
    //         }
    //     }
    //     return static_cast<usize>(this->length() - other.length());
    // }

    // 迭代器接口
    const_iterator begin() const noexcept { return begin_; }
    const_iterator end() const noexcept { return end_; }
    const_iterator cbegin() const noexcept { return begin_; }
    const_iterator cend() const noexcept { return end_; }

    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
    const_reverse_iterator crbegin() const { return rbegin(); }
    const_reverse_iterator crend() const { return rend(); }

private:
    /**
     * @brief 获取去除首尾空白后的索引范围
     * @return 首尾空白后的索引范围
     */
    Pair<usize, usize> get_trim_index() const {
        usize l = 0, r = length();
        while (l < r && (*this)[l].is_blank()) ++l;
        while (l < r && (*this)[r - 1].is_blank()) --r;
        return {l, r};
    }

    /**
     * @brief 获取去除首尾模式后的索引范围
     * @param pattern 要去除的模式
     * @return 首尾模式后的索引范围
     */
    Pair<usize, usize> get_trim_index(const Self pattern) const {
        usize l = 0, r = length(), p_size = pattern.length();
        while (l + p_size <= r && slice(l, l + p_size) == pattern) l += p_size;
        while (l + p_size <= r && slice(r - p_size, r) == pattern) r -= p_size;
        return {l, r};
    }

    /**
     * @brief 获取去除首部空白后的索引
     * @return 去除首部空白后的索引
     */
    usize get_ltrim_index() const {
        usize l = 0;
        const auto r = length();
        while (l < r && (*this)[l].is_blank()) ++l;
        return l;
    }

    /**
     * @brief 获取去除首部模式后的索引
     * @param pattern 要去除的模式
     * @return 去除首部模式后的索引
     */
    usize get_ltrim_index(const Self pattern) const {
        usize l = 0;
        const auto r = length(), p_size = pattern.length();
        while (l + p_size <= r && slice(l, l + p_size) == pattern) l += p_size;
        return l;
    }

    /**
     * @brief 获取去除尾部空白后的索引
     * @return 去除尾部空白后的索引
     */
    usize get_rtrim_index() const {
        constexpr usize l = 0;
        auto r = length();
        while (l < r && (*this)[r - 1].is_blank()) --r;
        return r;
    }

    /**
     * @brief 获取去除尾部模式后的索引
     * @param pattern 要去除的模式
     * @return 去除尾部模式后的索引
     */
    usize get_rtrim_index(const Self pattern) const {
        const usize l = 0, p_size = pattern.length();
        auto r = length();
        while (l + p_size <= r && slice(r - p_size, r) == pattern) r -= p_size;
        return r;
    }

    /**
     * @brief KMP辅助函数，求next数组
     * @param pattern 模式串
     * @note next[i]: 模式串[0, i)中最长相等前后缀的长度为next[i]
     * @note 时间复杂度为 O(m)，m为模式串的长度
     */
    static Vec<usize> get_next(const Self pattern) {
        const auto p_size = pattern.length();
        Vec<usize> next(p_size, 0);
        for (usize i = 1, j = 0; i < p_size; ++i) {
            // 失配，j按照next数组回跳
            while (j > 0 && pattern[i] != pattern[j]) {
                j = next[j - 1];
            }
            j += pattern[i] == pattern[j]; // 匹配，j前进
            next[i] = j;
        }
        return next;
    }

private:
    Iter begin_{};       // 视图起始迭代器
    Iter end_{};         // 视图结束迭代器（尾后）
    Encoding* encoding_; // 编码信息（不拥有所有权）
};

/**
 * @brief 推导指南
 */
template <typename Iter>
BasicStringView(Iter, Iter) -> BasicStringView<Iter>;

template <typename S>
BasicStringView(const S&) -> BasicStringView<typename S::const_iterator>;

template <typename S>
BasicStringView(const S&, usize, usize) -> BasicStringView<typename S::const_iterator>;

} // namespace my::util

#endif // STRING_VIEW_HPP
