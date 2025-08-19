/**
 * @brief 字符串构建器
 * @author Ricky
 * @date 2025/7/14
 * @version 2.0
 */
#ifndef STRING_BUILDER_HPP
#define STRING_BUILDER_HPP

#include "String.hpp"

namespace my::util {

/**
 * @class StringBuilder
 * @brief 字符串构建器
 */
class StringBuilder : public Object<StringBuilder> {
public:
    using Self = StringBuilder;

    /**
     * @brief 构造函数，指定初始容量和编码类型
     * @param initial_capacity 初始预分配容量，减少扩容次数
     * @param enc 字符串编码类型，默认为 UTF8
     */
    explicit StringBuilder(const usize initial_capacity = 64, const EncodingType enc = EncodingType::UTF8) :
            encoding_(encoding_map(enc)) {
        buf_.reserve(initial_capacity);
    }

    /**
     * @brief 查找模式串的第一个匹配位置
     * @param pattern 模式串，长度为m
     * @param pos 起始查找位置（可选）
     * @return 模式串的第一个匹配位置，未找到返回 `npos`
     * @note KMP算法，时间复杂度 O(n + m)，n为文本串的长度
     */
    usize find(const StringView& pattern, const usize pos = 0) const {
        return StringAlgorithm::kmp_find(buf_.begin() + pos, buf_.end(), pattern.begin(), pattern.end());
    }

    /**
     * @brief 查找模式串的所有匹配位置
     * @param pattern 模式串，长度为m
     * @return 所有匹配位置
     * @note KMP算法，时间复杂度 O(n + m)，n为文本串的长度
     */
    Vec<usize> find_all(const StringView& pattern) const {
        return StringAlgorithm::kmp_find_all(buf_.begin(), buf_.end(), pattern.begin(), pattern.end());
    }

    /**
     * @brief 追加 String 对象到构建器
     * @param str 要追加的字符串
     * @return 构建器自身引用
     */
    Self& append(const StringView& str) {
        if (!str.empty()) {
            buf_.reserve(buf_.size() + str.size());
            buf_.extend(str);
        }
        return *this;
    }

    /**
     * @brief 追加 CString 到构建器
     * @param cstr 要追加的 CString
     * @return 构建器自身引用
     */
    Self& append(const CStringView& cstr) {
        buf_.reserve(buf_.size() + cstr.length());
        buf_.extend(cstr);
        return *this;
    }

    /**
     * @brief 追加单个字符
     * @param ch 要追加的字符
     * @return 构建器自身引用
     */
    Self& append(const char ch) {
        buf_.append(CodePoint(ch));
        return *this;
    }

    /**
     * @brief 追加单个码点
     * @param cp 要追加的码点
     * @return 构建器自身引用
     */
    Self& append(const CodePoint& cp) {
        buf_.append(cp);
        return *this;
    }

    /**
     * @brief 追加 C 风格字符串
     * @param str 要追加的 C 字符串
     * @return 构建器自身引用
     */
    Self& append(const char* str) {
        const auto len = std::strlen(str);
        buf_.reserve(buf_.size() + len);
        for (usize i = 0; i < len; ++i) {
            buf_.append(CodePoint(str[i]));
        }
        return *this;
    }

    /**
     * @brief 追加格式化字符串
     * @tparam Args 格式化参数类型
     * @param fmt 格式化字符串
     * @param args 格式化参数
     * @return 构建器自身引用
     */
    template <typename... Args>
    Self& append_format(std::format_string<Args...> fmt, Args&&... args) {
        const auto formatted = std::format(fmt, std::forward<Args>(args)...);
        return append(CStringView(formatted.c_str(), formatted.length()));
    }

    /**
     * @brief 追加多个相同码点
     * @param cp 码点
     * @param count 码点数量
     * @return 构建器自身引用
     */
    Self& append_n(const CodePoint& cp, const usize count) {
        buf_.reserve(buf_.size() + count);
        for (usize i = 0; i < count; ++i) {
            buf_.append(cp);
        }
        return *this;
    }

    /**
     * @brief 追加码点数组
     * @param cps 码点数组
     * @param cnt 码点数组长度
     * @return 构建器自身引用
     */
    Self& append_array(const CodePoint* cps, const usize cnt) {
        buf_.reserve(buf_.size() + cnt);
        for (usize i = 0; i < cnt; ++i) {
            buf_.append(cps[i]);
        }
        return *this;
    }

    /**
     * @brief 构建最终字符串（拷贝语义）
     * @return 构建好的 String 对象
     * @note 内部会复制码点数组，原构建器可继续使用
     */
    [[nodiscard]] String build() const {
        Vec tmp(buf_);
        auto [size, code_points] = tmp.separate();
        return {code_points, size, encoding_};
    }

    /**
     * @brief 构建最终字符串（移动语义）
     * @return 构建好的 String 对象
     * @note 内部会移动码点数组，原构建器不能继续使用
     */
    [[nodiscard]] String build_move() {
        auto [size, code_points] = buf_.separate();
        return {code_points, size, encoding_};
    }

    /**
     * @brief 清空构建器内容
     */
    void clear() noexcept {
        buf_.clear();
    }

    /**
     * @brief 预留存储空间，减少扩容次数
     * @param new_cap 新容量
     */
    void reserve(const usize new_cap) {
        buf_.reserve(new_cap);
    }

    /**
     * @brief 获取当前码点数量
     * @return 当前存储的码点数量
     */
    usize size() const noexcept {
        return buf_.size();
    }

    /**
     * @brief 判断构建器是否为空
     * @return 是否为空
     */
    bool empty() const noexcept {
        return buf_.empty();
    }

private:
    Vec<CodePoint> buf_; // 码点缓冲区，存储所有追加内容
    Encoding* encoding_; // 字符串编码类型
};

} // namespace my::util

#endif // STRING_BUILDER_HPP