/**
 * @brief 字符串构建器
 * @author Ricky
 * @date 2025/5/6
 * @version 1.0
 */
#ifndef STRING_BUILDER_HPP
#define STRING_BUILDER_HPP

#include "Exception.hpp"
#include "Vec.hpp"
#include "String.hpp"
#include <cstring>
#include <format>

namespace my::util {

/**
 * @brief 字符串构建器 TODO 性能极差，可能修改为直接添加码点会好些
 * @class StringBuilder
 */
class StringBuilder : public Object<StringBuilder> {
public:
    using Self = StringBuilder;

    /**
     * @brief 构造函数
     * @param initial_capacity 初始缓冲区容量
     * @param encoding 字符串编码（默认UTF-8）
     */
    explicit StringBuilder(usize initial_capacity = 1024, util::Encoding* encoding = util::encoding_map(util::UTF8)) :
            encoding_(encoding) {
        char_buf_.reserve(initial_capacity);
    }

    /**
     * @brief 追加字符串
     * @note 允许链式调用
     */
    Self& append(const String& str) {
        check_encoding(str.encoding());

        // 直接获取String的底层字节表示并添加到缓冲区
        CString cs = str.__str__();
        char_buf_.reserve(char_buf_.size() + cs.size());
        for (usize i = 0; i < cs.size(); ++i) {
            char_buf_.append(cs[i]);
        }
        return *this;
    }

    Self& append(String&& str) {
        check_encoding(str.encoding());

        // 直接获取String的底层字节表示并添加到缓冲区
        CString cs = str.__str__();
        char_buf_.reserve(char_buf_.size() + cs.size());
        for (usize i = 0; i < cs.size(); ++i) {
            char_buf_.append(cs[i]);
        }
        return *this;
    }

    /**
     * @brief 追加 C 字符串
     * @note 允许链式调用
     */
    Self& append(const char* cstr) {
        if (!cstr) return *this;

        const usize len = std::strlen(cstr);
        char_buf_.reserve(char_buf_.size() + len);

        // 直接批量添加字节
        for (usize i = 0; i < len; ++i) {
            char_buf_.append(cstr[i]);
        }
        return *this;
    }

    /**
     * @brief 追加字符串视图
     */
    Self& append(std::string_view sv) {
        if (sv.empty()) return *this;

        const usize len = sv.length();
        char_buf_.reserve(char_buf_.size() + len);

        // 直接批量添加字节
        for (usize i = 0; i < len; ++i) {
            char_buf_.append(sv[i]);
        }
        return *this;
    }

    /**
     * @brief 追加单个码点
     * @note 允许链式调用
     */
    Self& append(const CodePoint& cp) {
        const usize size = cp.size();
        char_buf_.reserve(char_buf_.size() + size);

        // 直接添加码点的字节表示
        for (usize i = 0; i < size; ++i) {
            char_buf_.append(cp.data()[i]);
        }
        return *this;
    }

    Self& append(CodePoint&& cp) {
        const usize size = cp.size();
        char_buf_.reserve(char_buf_.size() + size);

        // 直接添加码点的字节表示
        for (usize i = 0; i < size; ++i) {
            char_buf_.append(cp.data()[i]);
        }
        return *this;
    }

    /**
     * @brief 追加单个字符
     */
    Self& append(char ch) {
        char_buf_.append(ch);
        return *this;
    }

    /**
     * @brief 追加整数
     */
    Self& append(i32 value) {
        char buffer[16];
        int len = std::snprintf(buffer, sizeof(buffer), "%d", value);
        return append(buffer, len);
    }

    /**
     * @brief 追加无符号整数
     */
    Self& append(u32 value) {
        char buffer[16];
        int len = std::snprintf(buffer, sizeof(buffer), "%u", value);
        return append(buffer, len);
    }

    /**
     * @brief 追加64位整数
     */
    Self& append(i64 value) {
        char buffer[24];
        int len = std::snprintf(buffer, sizeof(buffer), "%lld", (long long)value);
        return append(buffer, len);
    }

    /**
     * @brief 追加无符号64位整数
     */
    Self& append(u64 value) {
        char buffer[24];
        int len = std::snprintf(buffer, sizeof(buffer), "%llu", (unsigned long long)value);
        return append(buffer, len);
    }

    /**
     * @brief 追加特定长度的C字符串
     */
    Self& append(const char* str, usize len) {
        if (!str || len == 0) return *this;

        char_buf_.reserve(char_buf_.size() + len);
        for (usize i = 0; i < len; ++i) {
            char_buf_.append(str[i]);
        }
        return *this;
    }

    /**
     * @brief 追加格式化字符串
     * @tparam Args 格式化参数类型
     * @param format 格式化字符串
     * @param args 格式化参数
     */
    template <typename... Args>
    Self& append_format(std::string_view format, Args&&... args) {
        std::string formatted = std::vformat(format, std::make_format_args(std::forward<Args>(args)...));
        return append(formatted);
    }

    /**
     * @brief 构建最终的字符串
     */
    [[nodiscard]] String build() && {
        // 将字节缓冲区转换为码点数组
        auto cps = get_code_points(char_buf_.data(), char_buf_.size(), encoding_);
        auto [size, code_points] = cps.separate();
        return String(code_points, size, std::make_shared<StringManager>(size, code_points, encoding_));
    }

    /**
     * @brief 构建字符串并继续使用构建器
     */
    [[nodiscard]] String build() const& {
        // 将字节缓冲区转换为码点数组
        auto cps = get_code_points(char_buf_.data(), char_buf_.size(), encoding_);
        auto [size, code_points] = cps.separate();
        return String(code_points, size, std::make_shared<StringManager>(size, code_points, encoding_));
    }

    /**
     * @brief 清空构造器
     */
    void clear() noexcept {
        char_buf_.clear();
    }

    /**
     * @brief 预留空间
     * @param new_cap 新的容量
     */
    void reserve(usize new_cap) {
        char_buf_.reserve(new_cap);
    }

    /**
     * @brief 获取当前缓冲区大小
     */
    usize size() const noexcept {
        return char_buf_.size();
    }

    /**
     * @brief 判断缓冲区是否为空
     */
    bool empty() const noexcept {
        return char_buf_.empty();
    }

private:
    /**
     * @brief 检查编码是否匹配
     */
    void check_encoding(util::Encoding* other) const {
        if (other != encoding_) {
            throw runtime_exception("encoding mismatch in StringBuilder");
        }
    }

private:
    util::Vec<char> char_buf_; // 字符缓冲区
    util::Encoding* encoding_; // 字符串编码
};

} // namespace my::util

#endif // STRING_BUILDER_HPP