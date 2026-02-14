/**
 * @brief 字符串视图
 * @author Ricky
 * @date 2025/8/19
 * @version 1.0
 */

#ifndef STRING_VIEW_HPP
#define STRING_VIEW_HPP

#include "code_point.hpp"
#include "str_algorithm.hpp"

namespace my::util {

template <EncodingType Enc, typename Alloc>
class BasicString;

/**
 * @class BasicStringView
 * @brief 字符串视图只读视图  TODO Alloc
 * @note 源字符串生命周期必须不短于视图生命周期
 * @note 不推荐直接使用 BasicStringView，建议使用 StringView
 * @tparam Iter 迭代器类型
 */
template <typename Iter, EncodingType Enc = EncodingType::UTF8, typename Alloc = mem::Allocator<char>>
class BasicStringView : public Object<BasicStringView<Iter>> {
public:
    using Self = BasicStringView<Iter, Enc, Alloc>;
    using String = BasicString<Enc, Alloc>;
    using allocator_type = Alloc;
    using const_iterator = Iter;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    constexpr BasicStringView() noexcept = default;

    /**
     * @brief 通过首尾迭代器构造
     * @param begin 视图起始迭代器
     * @param end 视图尾后迭代器
     */
    BasicStringView(Iter begin, Iter end) noexcept :
            begin_(begin), end_(end) {}

    /**
     * @brief 通过字符串对象构造全视图
     */
    BasicStringView(const String& str) noexcept :
            begin_(str.cbegin()), end_(str.cend()) {}

    /**
     * @brief 通过绝对首索引和视图长度构造
     */
    BasicStringView(const String& str, usize pos, usize len) noexcept {
        const auto size = str.length();
        pos = std::min(pos, size);
        len = std::min(len, size - pos);

        begin_ = str.cbegin() + pos;
        end_ = begin_ + len;
    }

    auto to_f64() const -> f64 {
        return 0.0; // TODO
    }

    auto to_i64() const -> i64 {
        return 0; // TODO
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
     * @brief 获取编码类型
     */
    static constexpr EncodingType encoding() noexcept {
        return Enc;
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
        return Self(begin() + start, begin() + static_cast<usize>(end));
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
    usize find(const CodePoint<Enc>& c) const {
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
    usize find(const Self& pattern, const usize pos = 0) const {
        return StringAlgorithm::kmp_find(begin() + pos, end(), pattern.begin(), pattern.end());
    }

    /**
     * @brief 查找模式串的所有匹配位置
     * @param pattern 模式串，长度为m
     * @return 所有匹配位置
     * @note KMP算法，时间复杂度 O(n + m)，n为文本串的长度
     */
    Vec<usize> find_all(const Self& pattern) const {
        return StringAlgorithm::kmp_find_all(begin(), end(), pattern.begin(), pattern.end());
    }

    /**
     * @brief 检查字符串是否以指定子字符串开头
     * @param prefix 要检查的子字符串
     * @return 是否以指定子字符串开头
     */
    bool starts_with(const Self& prefix) const {
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
    bool ends_with(const Self& suffix) const {
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
    Self trim(const Self& pattern) const {
        auto [l, r] = get_trim_index(pattern);
        return slice(l, r);
    }

    /**
     * @brief 去除字符串首部的指定模式
     * @param pattern 要去除的模式
     * @return 去除模式后的字符串
     */
    Self ltrim(const Self& pattern) const {
        return slice(get_ltrim_index(pattern));
    }

    /**
     * @brief 去除字符串尾部的指定模式
     * @param pattern 要去除的模式
     * @return 去除模式后的字符串
     */
    Self rtrim(const Self& pattern) const {
        return slice(get_rtrim_index(pattern));
    }

    /**
     * @brief 将当前字符串以pattern作为分隔符分割
     * @param pattern 分隔符
     * @param max_split 最大分割次数（可选，-1表示无限制）
     * @return 分割后的字符串向量
     */
    Vec<Self> split(const Self& pattern, const isize max_split = -1) const {
        Vec<Self> res;
        usize start = 0;
        usize split_cnt = 0;
        const auto m_size = length(), p_size = pattern.length();
        const auto actual_splits = (max_split < 0) ? m_size : std::min(static_cast<usize>(max_split), m_size);

        // 空模式处理：按每个字符分割
        if (pattern.empty()) {
            for (usize i = 0; i < actual_splits; ++i) {
                res.append(Self(operator[](i)));
            }
            if (actual_splits < m_size) {
                res.append(slice(actual_splits));
            }
            return res;
        }

        const auto positions = find_all(pattern);
        for (const auto& pos : positions) {
            if (max_split >= 0 && split_cnt >= actual_splits) {
                break;
            }
            if (pos >= start && pos <= m_size) {
                res.append(slice(start, pos));
                start = pos + p_size;
                split_cnt++;
            }
        }

        // 添加最后一段
        res.append(slice(start));

        return res;
    }

    /**
     * @brief 转换为完整字符串，会拷贝数据
     */
    constexpr String to_string() const {
        return String{begin_, end_};
    }

    /**
     * @brief 返回字符串的哈希值
     * @return 字符串的哈希值
     */
    [[nodiscard]] hash_t __hash__() const {
        return to_string().__hash__();
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

    [[nodiscard]] cmp_t __cmp__(const Self& other) const {
        auto min_size = std::min(this->length(), other.length());
        for (usize i = 0; i < min_size; ++i) {
            auto cmp = this->operator[](i).__cmp__(other[i]);
            if (cmp != 0) {
                return cmp;
            }
        }
        return static_cast<usize>(this->length() - other.length());
    }

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
    Pair<usize, usize> get_trim_index(const Self& pattern) const {
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
    usize get_ltrim_index(const Self& pattern) const {
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
    usize get_rtrim_index(const Self& pattern) const {
        const usize l = 0, p_size = pattern.length();
        auto r = length();
        while (l + p_size <= r && slice(r - p_size, r) == pattern) r -= p_size;
        return r;
    }

private:
    Iter begin_{}; // 视图起始迭代器
    Iter end_{};   // 视图结束迭代器（尾后）
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
