/**
 * @brief UTF-8 字符串，类似 rust 的 String
 * @author Ricky
 * @date 2026/2/16
 * @version 1.0
 */
#ifndef STR_STRING_HPP
#define STR_STRING_HPP

#include "string_view.hpp"
#include "string_utf8.hpp"
#include "my_concepts.hpp"

#include <charconv>

namespace my::str {

/**
 * @class BasicString
 * @brief 可变 UTF-8 字符串，类似 rust 的 String
 * @details 内部以 Vec<u8> 存储 UTF-8 编码的字节序列，末尾始终保留一个 '\0' 作为哨兵
 *          支持码点级 push/pop、迭代、查找、分割等操作，不可变操作委托给 StringView
 */
template <typename Alloc = mem::Allocator<u8>>
class BasicString : public Object<BasicString<Alloc>> {
public:
    using value_type = u8;
    static constexpr usize npos = StringView::npos;
    using Self = BasicString<Alloc>;

    using cstr_allocator = typename Alloc::template rebind<char>::other;
    struct CStrDeleter {
        cstr_allocator alloc{};
        usize size{0};

        void operator()(char* p) noexcept {
            if (!p) return;
            alloc.deallocate(p, size);
        }
    };
    using CStrPtr = std::unique_ptr<char[], CStrDeleter>;

    /**
     * @brief 默认构造函数，创建空字符串
     */
    BasicString() {
        buf_.push(0);
    }

    /**
     * @brief 使用指定分配器构造空字符串
     * @param alloc 内存分配器
     */
    explicit BasicString(const Alloc& alloc) : buf_(alloc) {
        buf_.push(0);
    }

    /**
     * @brief 根据 StringView 构造字符串
     * @param view 字符串视图
     */
    explicit BasicString(const StringView& view) {
        buf_.reserve(view.len() + 1);
        for (auto b : view.bytes()) {
            buf_.push(b);
        }
        buf_.push(0);
    }

    /**
     * @brief 根据 C 风格字符串构造字符串
     * @param s C 风格字符串
     */
    explicit BasicString(const char* s) :
            BasicString(StringView(s)) {}

    /**
     * @brief 根据字符指针和长度构造字符串
     * @param s 字符指针
     * @param len 字节长度
     */
    BasicString(const char* s, const usize len) :
            BasicString(StringView(s, len)) {}

    BasicString(const BasicString& other) = default;
    BasicString& operator=(const BasicString& other) = default;

    BasicString(BasicString&& other) noexcept = default;
    BasicString& operator=(BasicString&& other) noexcept = default;

    ~BasicString() = default;

    /**
     * @brief 从 i32 构造字符串
     * @param val 整数值
     * @return 字符串
     */
    static Self from_i32(const i32 val) {
        char buf[16];
        const auto len = std::snprintf(buf, sizeof(buf), "%d", val);
        return Self(StringView(reinterpret_cast<const u8*>(buf), static_cast<usize>(len)));
    }

    /**
     * @brief 从 u32 构造字符串
     * @param val 无符号整数值
     * @return 字符串
     */
    static Self from_u32(const u32 val) {
        char buf[16];
        const auto len = std::snprintf(buf, sizeof(buf), "%u", val);
        return Self(StringView(reinterpret_cast<const u8*>(buf), static_cast<usize>(len)));
    }

    /**
     * @brief 从 i64 构造字符串
     * @param val 整数值
     * @return 字符串
     */
    static Self from_i64(const i64 val) {
        char buf[32];
        const auto len = std::snprintf(buf, sizeof(buf), "%lld", static_cast<long long>(val));
        return Self(StringView(reinterpret_cast<const u8*>(buf), static_cast<usize>(len)));
    }

    /**
     * @brief 从 u64 构造字符串
     * @param val 无符号整数值
     * @return 字符串
     */
    static Self from_u64(const u64 val) {
        char buf[32];
        const auto len = std::snprintf(buf, sizeof(buf), "%llu", static_cast<unsigned long long>(val));
        return Self(StringView(reinterpret_cast<const u8*>(buf), static_cast<usize>(len)));
    }

    /**
     * @brief 从 f64 构造字符串
     * @param val 浮点数值（自动去除多余的 0）
     * @return 字符串
     */
    static Self from_f64(const f64 val) {
        char buf[32];
        const auto len = std::snprintf(buf, sizeof(buf), "%g", val);
        return Self(StringView(reinterpret_cast<const u8*>(buf), static_cast<usize>(len)));
    }

    /**
     * @brief 从 i32 构造字符串（指定分配器）
     * @param val 整数值
     * @param alloc 内存分配器
     * @return 字符串
     */
    static Self from_i32(const i32 val, const Alloc& alloc) {
        Self s(alloc);
        char buf[16];
        const auto len = std::snprintf(buf, sizeof(buf), "%d", val);
        s.push_str(StringView(reinterpret_cast<const u8*>(buf), static_cast<usize>(len)));
        return s;
    }

    /**
     * @brief 从 u32 构造字符串（指定分配器）
     * @param val 无符号整数值
     * @param alloc 内存分配器
     * @return 字符串
     */
    static Self from_u32(const u32 val, const Alloc& alloc) {
        Self s(alloc);
        char buf[16];
        const auto len = std::snprintf(buf, sizeof(buf), "%u", val);
        s.push_str(StringView(reinterpret_cast<const u8*>(buf), static_cast<usize>(len)));
        return s;
    }

    /**
     * @brief 从 i64 构造字符串（指定分配器）
     * @param val 整数值
     * @param alloc 内存分配器
     * @return 字符串
     */
    static Self from_i64(const i64 val, const Alloc& alloc) {
        Self s(alloc);
        char buf[32];
        const auto len = std::snprintf(buf, sizeof(buf), "%lld", static_cast<long long>(val));
        s.push_str(StringView(reinterpret_cast<const u8*>(buf), static_cast<usize>(len)));
        return s;
    }

    /**
     * @brief 从 u64 构造字符串（指定分配器）
     * @param val 无符号整数值
     * @param alloc 内存分配器
     * @return 字符串
     */
    static Self from_u64(const u64 val, const Alloc& alloc) {
        Self s(alloc);
        char buf[32];
        const auto len = std::snprintf(buf, sizeof(buf), "%llu", static_cast<unsigned long long>(val));
        s.push_str(StringView(reinterpret_cast<const u8*>(buf), static_cast<usize>(len)));
        return s;
    }

    /**
     * @brief 从 f64 构造字符串（指定分配器）
     * @param val 浮点数值（自动去除多余的 0）
     * @param alloc 内存分配器
     * @return 字符串
     */
    static Self from_f64(const f64 val, const Alloc& alloc) {
        Self s(alloc);
        char buf[32];
        const auto len = std::snprintf(buf, sizeof(buf), "%g", val);
        s.push_str(StringView(reinterpret_cast<const u8*>(buf), static_cast<usize>(len)));
        return s;
    }

    /**
     * @brief 获取字符串长度（字节数，不含 '\0'）
     * @return 字节长度
     */
    [[nodiscard]] usize len() const noexcept {
        return buf_.len() > 0 ? buf_.len() - 1 : 0;
    }

    /**
     * @brief 判断字符串是否为空
     * @return true=空 false=非空
     */
    [[nodiscard]] bool is_empty() const noexcept {
        return len() == 0;
    }

    /**
     * @brief 获取当前容量（字节数）
     * @return 容量
     */
    [[nodiscard]] usize capacity() const noexcept {
        return buf_.capacity() > 0 ? buf_.capacity() - 1 : 0;
    }

    /**
     * @brief 获取第一个字节
     * @return 第一个字节的引用
     */
    [[nodiscard]] u8& first() noexcept {
        return buf_.first();
    }

    /**
     * @brief 获取第一个字节（常量版本）
     * @return 第一个字节的 const 引用
     */
    [[nodiscard]] const u8& first() const noexcept {
        return buf_.first();
    }

    /**
     * @brief 获取最后一个字节（非 '\0' 哨兵）
     * @return 最后一个字节的引用
     */
    [[nodiscard]] u8& last() noexcept {
        return buf_.at(buf_.len() - 2);
    }

    /**
     * @brief 获取最后一个字节（常量版本）
     * @return 最后一个字节的 const 引用
     */
    [[nodiscard]] const u8& last() const noexcept {
        return buf_.at(buf_.len() - 2);
    }

    /**
     * @brief 预留容量
     * @param new_cap 新容量（字节）
     */
    void reserve(const usize new_cap) {
        buf_.reserve(new_cap + 1);
    }

    /**
     * @brief 清空字符串
     */
    void clear() {
        buf_.clear();
        buf_.push(0);
    }

    /**
     * @brief 获取底层字节指针
     * @return 字节指针
     */
    [[nodiscard]] const u8* as_bytes() const noexcept {
        return buf_.data();
    }

    /**
     * @brief 获取字符串视图
     * @return StringView
     */
    [[nodiscard]] StringView as_str() const noexcept {
        return StringView::from_null_terminated(as_cstr(), len());
    }

    /**
     * @brief 获取 C 风格字符串指针
     * @return C 风格字符串指针
     */
    [[nodiscard]] const char* as_cstr() const noexcept {
        return reinterpret_cast<const char*>(buf_.data());
    }

    /**
     * @brief 转换为 std::string
     * @return std::string
     */
    [[nodiscard]] std::string to_std_string() const {
        return std::string(as_cstr(), len());
    }

    /**
     * @brief 转换为 BasicString（浅拷贝，返回自身拷贝）
     * @return BasicString
     */
    [[nodiscard]] Self to_string() const {
        return Self(*this);
    }

    /**
     * @brief 计算字符串的哈希值
     * @return 哈希值
     */
    [[nodiscard]] auto hash() const -> hash_t {
        return bytes_hash(reinterpret_cast<const char*>(as_bytes()), len());
    }

    /**
     * @brief 比较两个字符串（字典序）
     * @param other 另一个字符串
     * @return 负数=小于，0=等于，正数=大于
     */
    [[nodiscard]] auto cmp(const Self& other) const -> cmp_t {
        const usize lhs_len = len();
        const usize rhs_len = other.len();
        const usize min_len = std::min(lhs_len, rhs_len);
        if (min_len > 0) {
            const auto rc = std::memcmp(as_bytes(), other.as_bytes(), min_len);
            if (rc != 0) return static_cast<cmp_t>(rc);
        }
        return static_cast<cmp_t>(lhs_len) - static_cast<cmp_t>(rhs_len);
    }

    [[nodiscard]] auto eq(const Self& other) const -> bool {
        return cmp(other) == 0;
    }

    auto operator==(const Self& other) const -> bool { return eq(other); }
    auto operator!=(const Self& other) const -> bool { return !eq(other); }
    auto operator<(const Self& other) const -> bool { return cmp(other) < 0; }
    auto operator<=(const Self& other) const -> bool { return cmp(other) <= 0; }
    auto operator>(const Self& other) const -> bool { return cmp(other) > 0; }
    auto operator>=(const Self& other) const -> bool { return cmp(other) >= 0; }

    /**
     * @brief 索引操作符（不会检查越界）
     * @param idx 索引
     * @return 索引处的字节引用
     */
    [[nodiscard]] u8& operator[](const usize idx) noexcept {
        return buf_[idx];
    }

    /**
     * @brief 索引操作符（常量版本，不会检查越界）
     * @param idx 索引
     * @return 索引处的 const 字节引用
     */
    [[nodiscard]] const u8& operator[](const usize idx) const noexcept {
        return buf_[idx];
    }

    /**
     * @brief 带越界检查的索引访问
     * @param idx 索引
     * @return 索引处的字节引用
     * @exception Exception 若下标越界，则抛出 index_out_of_bounds_exception
     */
    [[nodiscard]] u8& at(const usize idx) {
        if (idx >= len()) {
            throw index_out_of_bounds_exception("Index {} out of bounds [0..{}]", idx, len());
        }
        return buf_[idx];
    }

    /**
     * @brief 带越界检查的索引访问（常量版本）
     * @param idx 索引
     * @return 索引处的 const 字节引用
     * @exception Exception 若下标越界，则抛出 index_out_of_bounds_exception
     */
    [[nodiscard]] const u8& at(const usize idx) const {
        if (idx >= len()) {
            throw index_out_of_bounds_exception("Index {} out of bounds [0..{}]", idx, len());
        }
        return buf_[idx];
    }

    /**
     * @brief 字符串切片 [start, end)
     * @param start 起始索引（包含）
     * @param end 结束索引（不包含）
     * @return 子字符串视图
     */
    [[nodiscard]] StringView slice(const usize start, const usize end) const noexcept {
        return as_str().slice(start, end);
    }

    /**
     * @brief 字符串切片 [start, len)
     * @param start 起始索引（包含）
     * @return 从 start 到末尾的子字符串视图
     */
    [[nodiscard]] StringView slice(const usize start) const noexcept {
        return as_str().slice(start);
    }

    /**
     * @brief 字符串切片，截取从 start 开始 count 个字节
     * @param start 起始索引
     * @param count 字节数
     * @return 子字符串视图
     */
    [[nodiscard]] StringView substr(const usize start, const usize count) const noexcept {
        return as_str().slice(start, start + count);
    }

    /**
     * @brief 转换为独占的 C 风格字符串
     * @return 以 unique_ptr 管理的 C 风格字符串
     */
    [[nodiscard]] CStrPtr into_cstr() const {
        const usize size = len() + 1;
        cstr_allocator alloc{};
        char* out = alloc.allocate(size);
        std::memcpy(out, buf_.data(), size);
        return CStrPtr(out, CStrDeleter{alloc, size});
    }

    /**
     * @brief 在末尾追加一个 Unicode 码点
     * @param cp 要追加的码点
     */
    void push(const char32_t cp) {
        u8 bytes[4]{};
        const usize n = detail::encode_utf8(cp, bytes);
        buf_.pop();
        buf_.reserve(buf_.len() + n);
        for (usize i = 0; i < n; ++i) {
            buf_.push(bytes[i]);
        }
        buf_.push(0);
    }

    /**
     * @brief 在末尾追加字符串视图
     * @param view 要追加的字符串视图
     */
    void push_str(const StringView& view) {
        if (view.is_empty()) return;
        buf_.pop();
        buf_.reserve(buf_.len() + view.len());
        for (auto b : view.bytes()) {
            buf_.push(b);
        }
        buf_.push(0);
    }

    /**
     * @brief 弹出末尾的 Unicode 码点
     * @return Option，为空时返回 None
     * @exception runtime_exception 若包含非法 UTF-8 编码，则抛出运行时异常
     */
    Option<char32_t> pop() {
        if (len() == 0) {
            return Option<char32_t>::None();
        }
        usize i = len() - 1;
        while (i > 0 && (buf_.at(i) & 0xC0u) == 0x80u) {
            --i;
        }
        const u8* data = buf_.data();
        const u8* p = data + i;
        const u8* end = data + len();
        char32_t cp = 0;
        if (!detail::decode_next(p, end, cp)) {
            throw runtime_exception("Invalid UTF-8");
        }
        while (buf_.len() > i + 1) {
            buf_.pop();
        }
        buf_.pop();
        buf_.push(0);
        return Option<char32_t>::Some(cp);
    }

    /**
     * @brief 获取字节迭代器范围
     * @return BytesRange
     */
    StringView::BytesRange bytes() const {
        return as_str().bytes();
    }

    /**
     * @brief 获取码点迭代器范围
     * @return CharsRange
     */
    StringView::CharsRange chars() const {
        return as_str().chars();
    }

    /**
     * @brief 查找模式串在字符串中的第一个匹配位置
     * @param pat 模式串
     * @return Option，匹配时返回 Some(index)，不匹配返回 None
     */
    [[nodiscard]] Option<usize> find(const StringView& pat) const {
        return as_str().find(pat);
    }

    /**
     * @brief 判断字符串是否包含指定子串
     * @param pat 子串
     * @return true=包含 false=不包含
     */
    [[nodiscard]] bool contains(const StringView& pat) const {
        return as_str().contains(pat);
    }

    /**
     * @brief 判断字符串是否以指定前缀开头
     * @param prefix 前缀
     * @return true=是 false=否
     */
    [[nodiscard]] bool starts_with(const StringView& prefix) const {
        return as_str().starts_with(prefix);
    }

    /**
     * @brief 判断字符串是否以指定后缀结尾
     * @param suffix 后缀
     * @return true=是 false=否
     */
    [[nodiscard]] bool ends_with(const StringView& suffix) const {
        return as_str().ends_with(suffix);
    }

    /**
     * @brief 去除字符串首部的指定前缀
     * @param prefix 要去除的前缀
     * @return Option，去除后返回 Some(view)，不匹配返回 None
     */
    [[nodiscard]] Option<StringView> strip_prefix(const StringView& prefix) const {
        return as_str().strip_prefix(prefix);
    }

    /**
     * @brief 去除字符串尾部的指定后缀
     * @param suffix 要去除的后缀
     * @return Option，去除后返回 Some(view)，不匹配返回 None
     */
    [[nodiscard]] Option<StringView> strip_suffix(const StringView& suffix) const {
        return as_str().strip_suffix(suffix);
    }

    /**
     * @brief 以指定分隔符分割字符串
     * @param pat 分隔符
     * @return 分割后的字符串视图向量
     */
    util::Vec<StringView> split(const StringView& pat) const {
        return as_str().split(pat);
    }

    /**
     * @brief 以指定分隔符分割字符串（限制分割次数）
     * @param pat 分隔符
     * @param max_split 最大分割次数（-1 表示无限制）
     * @return 分割后的字符串视图向量
     */
    util::Vec<StringView> split(const StringView& pat, const isize max_split) const {
        return as_str().split(pat, max_split);
    }

    /**
     * @brief 按空白字符分割字符串
     * @return 分割后的字符串视图向量
     */
    [[nodiscard]] util::Vec<StringView> split_whitespace() const {
        return as_str().split_whitespace();
    }

    /**
     * @brief 按换行符分割字符串
     * @return 按行分割后的字符串视图向量
     */
    util::Vec<StringView> lines() const {
        return as_str().lines();
    }

    /**
     * @brief 去除字符串首尾的空白字符
     * @return 去除空白后的字符串视图
     */
    StringView trim() const {
        return as_str().trim();
    }

    /**
     * @brief 去除字符串首部的空白字符
     * @return 去除首部空白后的字符串视图
     */
    StringView trim_start() const {
        return as_str().trim_start();
    }

    /**
     * @brief 去除字符串尾部的空白字符
     * @return 去除尾部空白后的字符串视图
     */
    StringView trim_end() const {
        return as_str().trim_end();
    }

    /**
     * @brief 去除字符串首尾的指定模式
     * @param pattern 要去除的模式
     * @return 去除模式后的字符串视图
     */
    StringView trim_matches(const StringView& pattern) const {
        return as_str().trim_matches(pattern);
    }

    /**
     * @brief 去除字符串首部的指定模式
     * @param pattern 要去除的模式
     * @return 去除首部模式后的字符串视图
     */
    StringView trim_start_matches(const StringView& pattern) const {
        return as_str().trim_start_matches(pattern);
    }

    /**
     * @brief 去除字符串尾部的指定模式
     * @param pattern 要去除的模式
     * @return 去除尾部模式后的字符串视图
     */
    StringView trim_end_matches(const StringView& pattern) const {
        return as_str().trim_end_matches(pattern);
    }

    /**
     * @brief 替换字符串中的子串
     * @param from 要被替换的子串
     * @param to 替换的新子串
     * @return 替换后的 BasicString
     */
    Self replace(const StringView& from, const StringView& to) const {
        return as_str().replace(from, to);
    }

    /**
     * @brief 将字符串转换为全小写
     * @return 全小写的 BasicString
     */
    Self to_lowercase() const {
        return as_str().to_lowercase();
    }

    /**
     * @brief 将字符串转换为全大写
     * @return 全大写的 BasicString
     */
    Self to_uppercase() const {
        return as_str().to_uppercase();
    }

    /**
     * @brief 查找模式串的所有匹配位置
     * @param pat 模式串
     * @return 所有匹配位置的向量
     */
    [[nodiscard]] util::Vec<usize> match_indices(const StringView& pat) const {
        return as_str().match_indices(pat);
    }

    /**
     * @brief 字符串拼接操作符
     * @param other 要拼接的字符串视图
     * @return 拼接后的新字符串
     */
    Self operator+(const StringView& other) const {
        Self res;
        res.reserve(len() + other.len());
        res.push_str(as_str());
        res.push_str(other);
        return res;
    }

    /**
     * @brief 累加字符串拼接操作符
     * @param other 要拼接的字符串视图
     * @return 自身的引用
     */
    Self& operator+=(const StringView& other) {
        push_str(other);
        return *this;
    }

    /**
     * @brief 字符串复制操作符
     * @param n 复制的份数
     * @return 复制后的新字符串
     */
    Self operator*(const usize n) const {
        if (n == 0) return Self();
        Self res;
        res.reserve(len() * n);
        for (usize i = 0; i < n; ++i) {
            res.push_str(as_str());
        }
        return res;
    }

    /**
     * @brief 重复字符串 n 次
     * @param n 重复次数
     * @return 重复后的新字符串
     */
    Self repeat(const usize n) const {
        return (*this) * n;
    }

    /**
     * @brief 以当前字符串为分隔符连接可迭代对象的每个元素
     * @tparam I 可迭代对象类型
     * @param iter 可迭代对象
     * @return 连接后的字符串
     */
    template <Iterable I>
    Self join(const I& iter) const {
        if (iter.len() == 0) {
            return Self();
        }
        Self res;
        bool first = true;
        for (auto&& elem : iter) {
            if (!first) {
                res.push_str(as_str());
            }
            first = false;
            using E = std::decay_t<decltype(elem)>;
            if constexpr (std::is_same_v<E, char32_t>) {
                res.push(elem);
            } else if constexpr (requires { elem.as_str(); }) {
                res.push_str(elem.as_str());
            } else if constexpr (std::is_constructible_v<StringView, const E&>) {
                res.push_str(StringView(elem));
            } else if constexpr (requires { my::cstr(elem); }) {
                auto s = my::cstr(elem);
                res.push_str(StringView(s));
            } else {
                auto s = std::to_string(elem);
                res.push_str(StringView(s));
            }
        }
        return res;
    }

    /**
     * @brief 查找第一个成对出现的括号，返回包含两个字符的子字符串
     * @param left 左括号码点
     * @param right 右括号码点
     * @return 包含括号及其内容的子字符串
     * @exception Exception 若左右括号数量不匹配，则抛出 runtime_exception
     */
    Self match_brackets(const char32_t left, const char32_t right) const {
        usize left_cp_idx = npos;
        for (auto [idx, cp] : chars().enumerate()) {
            if (cp == left) {
                left_cp_idx = idx;
                break;
            }
        }
        if (left_cp_idx == npos) return Self();

        usize match_cnt = 1;
        usize right_cp_idx = npos;
        for (auto [idx, cp] : chars().enumerate()) {
            if (idx <= left_cp_idx) continue;
            if (cp == right) {
                if (--match_cnt == 0) {
                    right_cp_idx = idx;
                    break;
                }
            } else if (cp == left) {
                ++match_cnt;
            }
        }
        if (right_cp_idx == npos) {
            throw runtime_exception("Unmatched brackets, too many left brackets");
        }

        usize left_byte = 0;
        usize right_byte = len();
        usize idx = 0;
        const u8* p = as_bytes();
        const u8* end_p = p + len();
        while (p < end_p) {
            if (idx == left_cp_idx) {
                left_byte = static_cast<usize>(p - as_bytes());
            }
            char32_t cp = 0;
            if (!detail::decode_next(p, end_p, cp)) {
                throw runtime_exception("Invalid UTF-8");
            }
            if (idx == right_cp_idx) {
                right_byte = static_cast<usize>(p - as_bytes());
                break;
            }
            ++idx;
        }
        return Self(StringView(as_bytes() + left_byte, right_byte - left_byte));
    }

    /**
     * @brief 删除字符串中所有指定码点
     * @param cp 要删除的码点
     * @return 删除后的字符串
     */
    Self remove_all(const char32_t cp) const {
        Self res;
        for (auto c : chars()) {
            if (c != cp) {
                res.push(c);
            }
        }
        return res;
    }

    /**
     * @brief 删除字符串中所有满足谓词的码点
     * @tparam F 谓词类型，接受 char32_t 参数，返回 bool
     * @param pred 谓词
     * @return 删除后的字符串
     */
    template <typename F>
    Self remove_all(F&& pred) const {
        Self res;
        for (auto c : chars()) {
            if (!pred(c)) {
                res.push(c);
            }
        }
        return res;
    }

    /**
     * @brief 保留字符串中满足谓词的码点（移除不满足的）
     * @tparam F 谓词类型，接受 char32_t 参数，返回 bool
     * @param pred 谓词
     */
    template <typename F>
    void retain(F&& pred) {
        Self res;
        for (auto c : chars()) {
            if (pred(c)) {
                res.push(c);
            }
        }
        *this = std::move(res);
    }

    /**
     * @brief 转换为字节向量（左值引用版本，深拷贝）
     * @return 字节向量
     */
    util::Vec<u8, Alloc> into_bytes() const& {
        return util::Vec<u8, Alloc>(buf_.slice(0, len()));
    }

    /**
     * @brief 转换为字节向量（右值引用版本，零拷贝）
     * @return 字节向量
     */
    util::Vec<u8, Alloc> into_bytes() && {
        auto result = util::Vec<u8, Alloc>(std::move(buf_));
        if (!result.is_empty() && result.at(result.len() - 1) == 0) {
            result.pop();
        }
        return result;
    }

    /**
     * @brief 交换两个字符串的内容
     * @param other 另一个字符串
     */
    void swap(Self& other) noexcept {
        buf_.swap(other.buf_);
    }

    /**
     * @brief 使用格式化字符串追加内容
     * @tparam Args 格式化参数类型
     * @param fmt 格式化字符串
     * @param args 格式化参数
     * @return 自身的引用
     */
    template <typename... Args>
    Self& push_format(std::format_string<Args...> fmt, Args&&... args) {
        const auto formatted = std::format(fmt, std::forward<Args>(args)...);
        push_str(StringView(formatted.c_str(), formatted.length()));
        return *this;
    }

    /**
     * @brief 追加指定码点 n 次
     * @param cp 要追加的码点
     * @param count 重复次数
     * @return 自身的引用
     */
    Self& push_n(const char32_t cp, const usize count) {
        for (usize i = 0; i < count; ++i) {
            push(cp);
        }
        return *this;
    }

    /**
     * @brief 收缩容量以适应当前大小
     */
    void shrink_to_fit() {
        if (buf_.capacity() > buf_.len()) {
            util::Vec<u8, Alloc> new_buf;
            new_buf.reserve(buf_.len());
            for (usize i = 0; i < buf_.len(); ++i) {
                new_buf.push(buf_[i]);
            }
            buf_ = std::move(new_buf);
        }
    }

    /**
     * @brief 获取首字节指针（用于范围 for 循环）
     * @return 首字节指针
     */
    [[nodiscard]] const u8* begin() const noexcept {
        return buf_.data();
    }

    /**
     * @brief 获取尾后字节指针（用于范围 for 循环）
     * @return 尾后字节指针
     */
    [[nodiscard]] const u8* end() const noexcept {
        return buf_.data() + len();
    }

    /**
     * @brief 字符串切片，支持负索引
     * @param start 起始索引（支持负数）
     * @param end 结束索引（支持负数）
     * @return 子字符串视图
     */
    [[nodiscard]] StringView slice(const usize start, isize end) const noexcept {
        return as_str().slice(start, end);
    }

private:
    util::Vec<u8, Alloc> buf_{};
};

/**
 * @brief String 类型别名
 */
using String = BasicString<mem::Allocator<u8>>;

template <typename AllocL, typename AllocR>
inline bool operator==(const BasicString<AllocL>& lhs, const BasicString<AllocR>& rhs) {
    return lhs.as_str() == rhs.as_str();
}

template <typename Alloc>
inline bool operator==(const BasicString<Alloc>& lhs, const StringView rhs) {
    return lhs.as_str() == rhs;
}

template <typename Alloc>
inline bool operator==(const StringView lhs, const BasicString<Alloc>& rhs) {
    return lhs == rhs.as_str();
}

template <typename Alloc>
inline bool operator==(const BasicString<Alloc>& lhs, const char* rhs) {
    return lhs.as_str() == rhs;
}

template <typename Alloc>
inline bool operator==(const char* lhs, const BasicString<Alloc>& rhs) {
    return lhs == rhs.as_str();
}

template <typename Alloc>
inline bool operator!=(const BasicString<Alloc>& lhs, const StringView rhs) {
    return !(lhs == rhs);
}

template <typename Alloc>
inline bool operator!=(const StringView lhs, const BasicString<Alloc>& rhs) {
    return !(lhs == rhs);
}

template <typename Alloc>
inline bool operator!=(const BasicString<Alloc>& lhs, const char* rhs) {
    return !(lhs == rhs);
}

template <typename Alloc>
inline bool operator!=(const char* lhs, const BasicString<Alloc>& rhs) {
    return !(lhs == rhs);
}

/**
 * @brief BasicString 与 BasicString 拼接
 */
template <typename AllocL, typename AllocR>
inline BasicString<AllocL> operator+(const BasicString<AllocL>& lhs, const BasicString<AllocR>& rhs) {
    BasicString<AllocL> res;
    res.reserve(lhs.len() + rhs.len());
    res.push_str(lhs.as_str());
    res.push_str(rhs.as_str());
    return res;
}

/**
 * @brief BasicString 与 const char* 拼接
 */
template <typename Alloc>
inline BasicString<Alloc> operator+(const BasicString<Alloc>& lhs, const char* rhs) {
    BasicString<Alloc> res;
    res.reserve(lhs.len() + std::strlen(rhs));
    res.push_str(lhs.as_str());
    res.push_str(StringView(rhs));
    return res;
}

/**
 * @brief const char* 与 BasicString 拼接
 */
template <typename Alloc>
inline BasicString<Alloc> operator+(const char* lhs, const BasicString<Alloc>& rhs) {
    BasicString<Alloc> res;
    res.reserve(std::strlen(lhs) + rhs.len());
    res.push_str(StringView(lhs));
    res.push_str(rhs.as_str());
    return res;
}

/**
 * @brief BasicString 与 StringView 拼接
 */
template <typename Alloc>
inline BasicString<Alloc> operator+(const BasicString<Alloc>& lhs, const StringView rhs) {
    BasicString<Alloc> res;
    res.reserve(lhs.len() + rhs.len());
    res.push_str(lhs.as_str());
    res.push_str(rhs);
    return res;
}

/**
 * @brief StringView 与 BasicString 拼接
 */
template <typename Alloc>
inline BasicString<Alloc> operator+(const StringView lhs, const BasicString<Alloc>& rhs) {
    BasicString<Alloc> res;
    res.reserve(lhs.len() + rhs.len());
    res.push_str(lhs);
    res.push_str(rhs.as_str());
    return res;
}

} // namespace my::str

namespace my {

/**
 * @brief 概念：类型 T 支持 to_string 转换为 str::String
 */
template <typename T>
concept ToString = requires(const T& t) {
    { to_string(t) } -> std::same_as<str::String>;
};

} // namespace my

template <typename Alloc>
struct std::formatter<my::str::BasicString<Alloc>, char> : std::formatter<std::string_view, char> {
    auto format(const my::str::BasicString<Alloc>& value, auto& ctx) const {
        auto view = value.as_str();
        return std::formatter<std::string_view, char>::format(
            std::string_view(reinterpret_cast<const char*>(view.as_bytes()), view.len()), ctx);
    }
};

#endif // STR_STRING_HPP
