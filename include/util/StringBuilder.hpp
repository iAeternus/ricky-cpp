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
     * @brief 追加 String 对象到构建器
     * @param str 要追加的字符串
     * @return 构建器自身引用
     */
    Self& append(const String& str) {
        if (str.size()) {
            buf_.reserve(buf_.size() + str.size());
            for (usize i = 0; i < str.size(); ++i) {
                buf_.append(str[i]);
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
        auto len = std::strlen(str);
        buf_.reserve(buf_.size() + len);
        for (usize i = 0; i < len; ++i) {
            buf_.append(CodePoint(str[i]));
        }
        return *this;
    }

    /**
     * @brief 追加格式化字符串
     * @tparam Args 格式化参数类型
     * @param format 格式化字符串
     * @param args 格式化参数
     * @return 构建器自身引用
     */
    template <typename... Args>
    Self& append_format(std::string_view format, Args&&... args) {
        std::string formatted = std::vformat(format, std::make_format_args(std::forward<Args>(args)...));
        return append(formatted.c_str());
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
    util::Vec<CodePoint> buf_; // 码点缓冲区，存储所有追加内容
    util::Encoding* encoding_; // 字符串编码类型
};

} // namespace my::util

#endif // STRING_BUILDER_HPP