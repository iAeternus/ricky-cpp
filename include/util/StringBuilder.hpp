/**
 * @brief 字符串构建器
 * @author Ricky
 * @date 2025/5/6
 * @version 1.0
 */
#ifndef STRING_BUILDER_HPP
#define STRING_BUILDER_HPP

#include "Vec.hpp"
#include "String.hpp"

namespace my::util {

/**
 * @brief 字符串构建器
 * @class StringBuilder
 */
class StringBuilder : public Object<StringBuilder> {
public:
    using Self = StringBuilder;

    /**
     * @brief 默认构造函数
     */
    explicit StringBuilder(util::Encoding* enc = util::encoding_map(util::UTF8)) :
            encoding_(enc) {}

    /**
     * @brief 追加字符串
     * @note 允许链式调用
     */
    Self& append(const String& str) {
        if (str.encoding() != encoding_) {
            EncodingError("Encoding mismatch");
        }
        for (usize i = 0; i < str.size(); ++i) {
            buf_.append(str.at(i));
        }
        return *this;
    }

    Self& append(String&& str) {
        if (str.encoding() != encoding_) {
            EncodingError("Encoding mismatch");
        }
        for (usize i = 0; i < str.size(); ++i) {
            buf_.append(std::move(str.at(i)));
        }
        return *this;
    }

    /**
     * @brief 追加 C 字符串
     * @note 允许链式调用
     */
    Self& append(const char* cstr) {
        usize len = std::strlen(cstr);
        usize i = 0;
        while (i < len) {
            CodePoint cp(cstr + i, encoding_);
            i += cp.size();
            buf_.append(std::move(cp)); // 移动语义
        }
        return *this;
    }

    /**
     * @brief 追加单个码点
     * @note 允许链式调用
     */
    Self& append(const CodePoint& cp) {
        buf_.append(cp);
        return *this;
    }

    Self& append(CodePoint&& cp) {
        buf_.append(std::move(cp));
        return *this;
    }

    /**
     * @brief 转换为String
     */
    String str() {
        auto length = buf_.size();
        auto [size, code_points] = buf_.separate();
        return String(code_points, length, std::make_shared<StringManager>(length, code_points, encoding_));
    }

    /**
     * @brief 清空构造器
     */
    void clear() {
        buf_.clear();
    }

private:
    util::Vec<CodePoint> buf_; // 缓冲区
    util::Encoding* encoding_; // 字符串编码
};

} // namespace my::util

#endif // STRING_BUILDER_HPP