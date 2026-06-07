/**
 * @brief 字符串视图，类似 rust 的 &str
 * @author Ricky
 * @date 2026/2/16
 * @version 1.0
 */
#ifndef STR_STRING_VIEW_HPP
#define STR_STRING_VIEW_HPP

#include "option.hpp"
#include "vec.hpp"

#include <charconv>

namespace my::str {

namespace detail {

/**
 * @brief 判断码点是否为 ASCII 空白字符
 * @param cp Unicode 码点
 * @return true=是空白 false=否
 */
bool is_ascii_whitespace(const char32_t cp);

/**
 * @brief 验证字节序列是否为合法 UTF-8
 * @param data 字节序列指针
 * @param len 字节序列长度
 * @exception runtime_exception 若包含非法 UTF-8 编码，则抛出运行时异常
 */
void validate_utf8(const u8* data, const usize len);

} // namespace detail

template <typename Alloc>
class BasicString;

/**
 * @class StringView
 * @brief UTF-8 字符串视图，类似 rust 的 &str
 * @details 不可变的 UTF-8 编码字符串引用，不拥有数据所有权
 *          支持字节级和码点级迭代、查找、切片、分割等操作
 */
class StringView : public Object<StringView> {
public:
    using value_type = u8;
    static constexpr usize npos = static_cast<usize>(-1);
    using Self = StringView;
    using cstr_allocator = mem::Allocator<char>;
    struct CStrDeleter {
        cstr_allocator alloc{};
        usize size{0};

        void operator()(char* p) noexcept;
    };
    using CStrPtr = std::unique_ptr<char[], CStrDeleter>;

    /**
     * @brief 默认构造函数，创建空视图
     */
    StringView() noexcept = default;

    /**
     * @brief 根据 C 风格字符串构造视图
     * @param s C 风格字符串（以 '\0' 结尾）
     */
    explicit StringView(const char* s);

    /**
     * @brief 根据字符指针和长度构造视图
     * @param s 字符指针
     * @param len 视图长度（字节）
     */
    StringView(const char* s, const usize len);

    /**
     * @brief 根据字节指针和长度构造视图
     * @param s 字节指针
     * @param len 视图长度（字节）
     */
    StringView(const u8* s, const usize len);

    /**
     * @brief 从以 '\0' 结尾的字符串创建视图（已知长度）
     * @param s C 风格字符串
     * @param len 字符串长度（不含 '\0'）
     * @return 字符串视图
     * @note 标记 cstr_backed_=true，优化 as_cstr() 等操作
     */
    static StringView from_null_terminated(const char* s, const usize len) noexcept;

    /**
     * @brief 获取字符串长度（字节数）
     * @return 字节长度
     */
    [[nodiscard]] usize len() const noexcept;

    /**
     * @brief 判断字符串是否为空
     * @return true=空 false=非空
     */
    [[nodiscard]] bool is_empty() const noexcept;

    /**
     * @brief 获取底层字节指针
     * @return 字节指针
     */
    [[nodiscard]] const u8* as_bytes() const noexcept;

    /**
     * @brief 返回自身（兼容 String 接口）
     * @return 自身的拷贝
     */
    [[nodiscard]] StringView as_str() const noexcept;

    /**
     * @brief 转换为 std::string_view
     * @return std::string_view
     */
    [[nodiscard]] std::string_view to_std_string_view() const noexcept;

    /**
     * @brief 转换为 std::string
     * @return std::string
     */
    [[nodiscard]] std::string to_std_string() const;

    /**
     * @brief 将字符串解析为 i64
     * @return 解析得到的 i64 值
     * @exception runtime_exception 若字符串不是合法整数，则抛出运行时异常
     */
    [[nodiscard]] i64 to_i64() const;

    /**
     * @brief 将字符串解析为 f64
     * @return 解析得到的 f64 值
     * @exception runtime_exception 若字符串不是合法浮点数，则抛出运行时异常
     */
    [[nodiscard]] f64 to_f64() const;

    /**
     * @brief 获取 C 风格字符串指针
     * @return C 风格字符串指针
     * @note 仅在 cstr_backed_=true 时有效
     */
    [[nodiscard]] const char* as_cstr() const noexcept;

    /**
     * @brief 转换为独占的 C 风格字符串
     * @return 以 unique_ptr 管理的 C 风格字符串
     */
    [[nodiscard]] CStrPtr into_cstr() const;

    /**
     * @brief 字节迭代器范围
     */
    struct BytesRange {
        const u8* begin_;
        const u8* end_;

        /**
         * @brief 字节迭代器
         */
        struct Iterator {
            const u8* p;

            bool operator!=(const Iterator& other) const { return p != other.p; }
            u8 operator*() const { return *p; }
            Iterator& operator++() {
                ++p;
                return *this;
            }
        };

        Iterator begin() const {
            return {begin_};
        }

        Iterator end() const {
            return {end_};
        }

        /**
         * @brief 获取字节数
         * @return 字节数
         */
        usize count() const {
            return static_cast<usize>(end_ - begin_);
        }

        /**
         * @brief 获取字节数
         * @return 字节数
         */
        usize size() const {
            return count();
        }

        /**
         * @brief 获取指定索引处的字节
         * @param idx 索引
         * @return Option，存在时返回 Some(byte)，索引越界返回 None
         */
        Option<u8> nth(const usize idx) const {
            const auto n = count();
            if (idx >= n) return Option<u8>::None();
            return Option<u8>::Some(static_cast<u8>(begin_[idx]));
        }

        /**
         * @brief 获取最后一个字节
         * @return Option，为空时返回 None
         */
        Option<u8> last() const {
            const auto n = count();
            if (n == 0) return Option<u8>::None();
            return Option<u8>::Some(static_cast<u8>(begin_[n - 1]));
        }

        /**
         * @brief 带索引的字节迭代器范围
         */
        struct EnumerateRange {
            const u8* begin_;
            const u8* end_;

            struct Iterator {
                const u8* p;
                usize idx;

                bool operator!=(const Iterator& other) const {
                    return p != other.p;
                }

                Pair<usize, u8> operator*() const {
                    return {idx, *p};
                }

                Iterator& operator++() {
                    ++p;
                    ++idx;
                    return *this;
                }
            };

            Iterator begin() const {
                return {begin_, 0};
            }

            Iterator end() const {
                return {end_, static_cast<usize>(end_ - begin_)};
            }
        };

        /**
         * @brief 创建带索引的枚举迭代器
         * @return EnumerateRange
         */
        EnumerateRange enumerate() const {
            return {begin_, end_};
        }
    };

    /**
     * @brief 码点迭代器范围
     */
    struct CharsRange {
        const u8* begin_;
        const u8* end_;

        /**
         * @brief 码点迭代器，按 UTF-8 解码遍历
         */
        struct Iterator {
            const u8* p;
            const u8* end;
            const u8* next;
            char32_t value;

            Iterator(const u8* cur, const u8* end_);

            bool operator!=(const Iterator& other) const {
                return p != other.p;
            }

            char32_t operator*() const {
                return value;
            }

            Iterator& operator++();
        };

        Iterator begin() const {
            return {begin_, end_};
        }

        Iterator end() const {
            return {end_, end_};
        }

        usize size() const {
            return count();
        }

        /**
         * @brief 获取码点数量
         * @return 码点数
         */
        usize count() const;

        /**
         * @brief 获取指定索引处的码点
         * @param idx 索引
         * @return Option，存在时返回 Some(codepoint)，越界返回 None
         */
        Option<char32_t> nth(usize idx) const;

        /**
         * @brief 获取最后一个码点
         * @return Option，为空时返回 None
         */
        Option<char32_t> last() const;

        /**
         * @brief 带索引的码点迭代器范围
         */
        struct EnumerateRange {
            const u8* begin_;
            const u8* end_;

            struct Iterator {
                const u8* p;
                const u8* end;
                const u8* next;
                char32_t value;
                usize idx;

                Iterator(const u8* cur, const u8* end_);

                bool operator!=(const Iterator& other) const {
                    return p != other.p;
                }

                Pair<usize, char32_t> operator*() const {
                    return {idx, value};
                }

                Iterator& operator++();
            };

            Iterator begin() const {
                return {begin_, end_};
            }

            Iterator end() const {
                return {end_, end_};
            }
        };

        /**
         * @brief 创建带索引的枚举迭代器
         * @return EnumerateRange
         */
        EnumerateRange enumerate() const {
            return {begin_, end_};
        }
    };

    /**
     * @brief 获取字节迭代器范围
     * @return BytesRange
     */
    BytesRange bytes() const;

    /**
     * @brief 获取码点迭代器范围
     * @return CharsRange
     */
    CharsRange chars() const;

    /**
     * @brief 查找模式串在字符串中的第一个匹配位置
     * @param pat 模式串
     * @return Option，匹配时返回 Some(index)，不匹配返回 None
     * @note 单字节使用 memchr，多字节使用 Two-Way 算法
     */
    [[nodiscard]] Option<usize> find(const StringView& pat) const;

    /**
     * @brief 查找模式串在字符串中的所有匹配位置
     * @param pat 模式串
     * @return 所有匹配位置的向量
     * @note 使用 KMP 算法循环查找，时间复杂度 O(n + m)
     */
    [[nodiscard]] util::Vec<usize> match_indices(const StringView& pat) const;

    /**
     * @brief 判断字符串是否包含指定子串
     * @param pat 子串
     * @return true=包含 false=不包含
     */
    [[nodiscard]] bool contains(const StringView& pat) const;

    /**
     * @brief 判断字符串是否以指定前缀开头
     * @param prefix 前缀
     * @return true=是 false=否
     */
    [[nodiscard]] bool starts_with(const StringView& prefix) const;

    /**
     * @brief 判断字符串是否以指定后缀结尾
     * @param suffix 后缀
     * @return true=是 false=否
     */
    [[nodiscard]] bool ends_with(const StringView& suffix) const;

    /**
     * @brief 去除字符串首部的指定前缀
     * @param prefix 要去除的前缀
     * @return Option，去除后返回 Some(view)，不匹配返回 None
     */
    [[nodiscard]] Option<StringView> strip_prefix(const StringView& prefix) const;

    /**
     * @brief 去除字符串尾部的指定后缀
     * @param suffix 要去除的后缀
     * @return Option，去除后返回 Some(view)，不匹配返回 None
     */
    [[nodiscard]] Option<StringView> strip_suffix(const StringView& suffix) const;

    /**
     * @brief 字符串切片 [start, end)
     * @param start 起始索引（包含）
     * @param end 结束索引（不包含）
     * @return 子字符串视图
     */
    [[nodiscard]] StringView slice(const usize start, const usize end) const noexcept;

    /**
     * @brief 字符串切片 [start, len)
     * @param start 起始索引（包含）
     * @return 从 start 到末尾的子字符串视图
     */
    [[nodiscard]] StringView slice(const usize start) const noexcept;

    /**
     * @brief 字符串切片，支持负索引
     * @param start 起始索引（支持负数，-1 表示最后一个字节）
     * @param end 结束索引（支持负数，-1 表示最后一个字节）
     * @return 子字符串视图
     */
    [[nodiscard]] StringView slice(const usize start, isize end) const noexcept;

    /**
     * @brief 索引操作符（不会检查越界）
     * @param idx 索引
     * @return 索引处的字节
     */
    [[nodiscard]] const u8& operator[](const usize idx) const noexcept;

    /**
     * @brief 带越界检查的索引访问
     * @param idx 索引
     * @return 索引处的字节
     * @exception Exception 若下标越界，则抛出 index_out_of_bounds_exception
     */
    [[nodiscard]] const u8& at(const usize idx) const;

    /**
     * @brief 获取首字节指针
     * @return 首字节指针
     */
    [[nodiscard]] const u8* begin() const noexcept;

    /**
     * @brief 获取尾后字节指针
     * @return 尾后字节指针
     */
    [[nodiscard]] const u8* end() const noexcept;

    /**
     * @brief 以指定分隔符分割字符串
     * @param pat 分隔符
     * @return 分割后的字符串视图向量
     */
    util::Vec<StringView> split(const StringView& pat) const;

    /**
     * @brief 以指定分隔符分割字符串（限制分割次数）
     * @param pat 分隔符
     * @param max_split 最大分割次数（-1 表示无限制）
     * @return 分割后的字符串视图向量
     */
    util::Vec<StringView> split(const StringView& pat, const isize max_split) const;

    /**
     * @brief 按空白字符分割字符串
     * @return 分割后的字符串视图向量
     */
    util::Vec<StringView> split_whitespace() const;

    /**
     * @brief 按换行符分割字符串
     * @return 按行分割后的字符串视图向量
     */
    util::Vec<StringView> lines() const;

    /**
     * @brief 去除字符串首尾的空白字符
     * @return 去除空白后的字符串视图
     */
    StringView trim() const;

    /**
     * @brief 去除字符串首部的空白字符
     * @return 去除首部空白后的字符串视图
     */
    StringView trim_start() const;

    /**
     * @brief 去除字符串尾部的空白字符
     * @return 去除尾部空白后的字符串视图
     */
    StringView trim_end() const;

    /**
     * @brief 去除字符串首尾的指定模式
     * @param pattern 要去除的模式
     * @return 去除模式后的字符串视图
     */
    StringView trim_matches(const StringView& pattern) const noexcept;

    /**
     * @brief 去除字符串首部的指定模式
     * @param pattern 要去除的模式
     * @return 去除首部模式后的字符串视图
     */
    StringView trim_start_matches(const StringView& pattern) const noexcept;

    /**
     * @brief 去除字符串尾部的指定模式
     * @param pattern 要去除的模式
     * @return 去除尾部模式后的字符串视图
     */
    StringView trim_end_matches(const StringView& pattern) const noexcept;

    /**
     * @brief 替换字符串中的子串
     * @param from 要被替换的子串
     * @param to 替换的新子串
     * @return 替换后的 BasicString
     */
    BasicString<mem::Allocator<u8>> replace(const StringView& from, const StringView& to) const;

    /**
     * @brief 将字符串转换为全小写
     * @return 全小写的 BasicString
     */
    BasicString<mem::Allocator<u8>> to_lowercase() const;

    /**
     * @brief 将字符串转换为全大写
     * @return 全大写的 BasicString
     */
    BasicString<mem::Allocator<u8>> to_uppercase() const;

    /**
     * @brief 转换为 BasicString（深拷贝）
     * @return BasicString
     */
    BasicString<mem::Allocator<u8>> to_string() const;

    /**
     * @brief 计算字符串的哈希值
     * @return 哈希值
     */
    [[nodiscard]] auto hash() const -> hash_t;

    /**
     * @brief 比较两个字符串（字典序）
     * @param other 另一个字符串视图
     * @return 负数=小于，0=等于，正数=大于
     */
    [[nodiscard]] auto cmp(const Self& other) const -> cmp_t;

    /**
     * @brief 判断两个字符串是否相等
     * @param other 另一个字符串视图
     * @return true=相等 false=不相等
     */
    [[nodiscard]] auto eq(const Self& other) const -> bool;

    auto operator==(const Self& other) const -> bool;
    auto operator!=(const Self& other) const -> bool;
    auto operator<(const Self& other) const -> bool;
    auto operator<=(const Self& other) const -> bool;
    auto operator>(const Self& other) const -> bool;
    auto operator>=(const Self& other) const -> bool;

private:
    const u8* data_{nullptr};
    usize len_{0};
    bool cstr_backed_{false};
};

/**
 * @brief StringView 与 const char* 相等比较
 */
inline bool operator==(const StringView lhs, const char* rhs) {
    return lhs == StringView(rhs);
}

/**
 * @brief const char* 与 StringView 相等比较
 */
inline bool operator==(const char* lhs, const StringView rhs) {
    return StringView(lhs) == rhs;
}

inline bool operator!=(const StringView lhs, const char* rhs) {
    return !(lhs == rhs);
}

inline bool operator!=(const char* lhs, const StringView rhs) {
    return !(lhs == rhs);
}

/**
 * @brief StringView 与 std::string_view 相等比较
 */
inline bool operator==(const StringView lhs, const std::string_view rhs) {
    return lhs == StringView(rhs.data(), rhs.size());
}

inline bool operator==(const std::string_view lhs, const StringView rhs) {
    return StringView(lhs.data(), lhs.size()) == rhs;
}

inline bool operator!=(const StringView lhs, const std::string_view rhs) {
    return !(lhs == rhs);
}

inline bool operator!=(const std::string_view lhs, const StringView rhs) {
    return !(lhs == rhs);
}

} // namespace my::str

template <>
struct std::formatter<my::str::StringView, char> : std::formatter<std::string_view, char> {
    auto format(const my::str::StringView value, auto& ctx) const {
        return std::formatter<std::string_view, char>::format(
            std::string_view(reinterpret_cast<const char*>(value.as_bytes()), value.len()), ctx);
    }
};

namespace my {

/**
 * @brief 字符串字面量 _sv 后缀，转换为 StringView
 * @param s 字符串指针
 * @param len 字符串长度
 * @return StringView
 */
inline str::StringView operator""_sv(const char* s, size_t len) noexcept {
    return str::StringView(s, static_cast<usize>(len));
}

} // namespace my

#endif // STR_STRING_VIEW_HPP
