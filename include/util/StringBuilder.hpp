/**
 * @brief 高性能字符串构建器，支持批量追加和高效拼接
 * @author Ricky
 * @date 2025/7/14
 * @version 2.0
 *
 * 用于高效构建 Unicode 字符串，避免频繁分配和拷贝，适合大量字符串拼接场景
 */
#ifndef STRING_BUILDER_HPP
#define STRING_BUILDER_HPP

#include "String.hpp"

namespace my::util {

/**
 * @class StringBuilder
 * @brief 高性能字符串构建器，支持批量追加和高效拼接
 *
 * 通过内部动态数组 Vec<CodePoint> 管理码点，避免频繁分配和拷贝
 * 适用于需要大量字符串拼接的场景
 */
class StringBuilder : public Object<StringBuilder> {
public:
    using Self = StringBuilder;

    /**
     * @brief 构造函数，指定初始容量和编码类型
     * @param initial_capacity 初始预分配容量，减少扩容次数
     * @param enc 字符串编码类型，默认为 UTF8
     */
    explicit StringBuilder(usize initial_capacity = 64, EncodingType enc = EncodingType::UTF8) :
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
    usize find(const String& pattern, usize pos = 0) {
        if (pattern.empty()) return npos;
        auto m_size = buf_.size(), p_size = pattern.size();
        auto next = get_next(pattern);
        for (usize i = pos, j = 0; i < m_size; ++i) {
            // 失配，j按照next回跳
            while (j > 0 && buf_[i] != pattern[j]) {
                j = next[j - 1];
            }
            j += (buf_[i] == pattern[j]); // 匹配，j前进
            // 模式串匹配完，返回文本串匹配起点
            if (j == p_size) {
                return i - p_size + 1;
            }
        }
        return npos;
    }

    /**
     * @brief 追加 String 对象到构建器
     * @param str 要追加的字符串
     * @return 构建器自身引用
     */
    Self& append(const String& str) {
        if (!str.empty()) {
            buf_.reserve(buf_.size() + str.size());
            for (const auto & cp : str) {
                buf_.append(cp);
            }
        }
        return *this;
    }

    /**
     * @brief 追加 CString 到构建器
     * @param cstr 要追加的 CString
     * @return 构建器自身引用
     */
    Self& append(const CString& cstr) {
        auto cps = get_code_points(cstr.data(), cstr.size(), encoding_);
        buf_.reserve(buf_.size() + cps.size());
        buf_.extend(cps);
        return *this;
    }

    /**
     * @brief 追加单个字符
     * @param ch 要追加的字符
     * @return 构建器自身引用
     */
    Self& append(char ch) {
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
        return append(formatted);
    }

    /**
     * @brief 构建最终字符串（拷贝语义）
     * @return 构建好的 String 对象
     * @note 内部会复制码点数组，原构建器可继续使用
     */
    [[nodiscard]] String build() const {
        Vec<CodePoint> tmp(buf_);
        auto [size, code_points] = tmp.separate();
        return String(code_points, size, encoding_);
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
    void reserve(usize new_cap) {
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
    /**
     * @brief KMP辅助函数，求next数组
     * @param pattern 模式串
     * @note next[i]: 模式串[0, i)中最长相等前后缀的长度为next[i]
     * @note 时间复杂度为 O(m)，m为模式串的长度
     */
    static Vec<usize> get_next(const String& pattern) {
        auto p_size = pattern.size();
        Vec<usize> next(p_size, 0);
        for (usize i = 1, j = 0; i < p_size; ++i) {
            // 失配，j按照next数组回跳
            while (j > 0 && pattern[i] != pattern[j]) {
                j = next[j - 1];
            }
            j += (pattern[i] == pattern[j]); // 匹配，j前进
            next[i] = j;
        }
        return next;
    }

private:
    Vec<CodePoint> buf_; // 码点缓冲区，存储所有追加内容
    Encoding* encoding_; // 字符串编码类型
};

} // namespace my::util

#endif // STRING_BUILDER_HPP