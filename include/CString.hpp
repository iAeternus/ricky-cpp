/**
 * @brief 字符串，解决char*不能做返回值的问题
 * @author Ricky
 * @date 2024/11/24
 * @version 1.0
 */
#ifndef CSTRING_HPP
#define CSTRING_HPP

#include "hash.hpp"
#include "Allocator.hpp"
#include "Function.hpp"

#include <cctype>
#include <cstring>
#include <format>
#include <sstream>

namespace my {

template <typename Alloc>
class BasicCString;

/**
 * @class CStringView
 * @brief C风格字符串视图
 * @details 减少拷贝次数，加速切片操作
 * @note 源字符串生命周期必须不短于视图生命周期
 */
class CStringView {
public:
    using Self = CStringView;

    /**
     * @brief 通过首尾指针构造
     * @param begin 指向切片首地址
     * @param end 指向切片尾后地址
     */
    CStringView(char* begin, char* end) :
            begin_(begin), end_(end) {}

    /**
     * @brief 通过首指针和切片长度构造
     * @param begin 指向切片首地址
     * @param size 切片长度
     */
    CStringView(char* begin, const usize size) :
            begin_(begin), end_(begin_ + size) {}

    /**
     * @brief 字符串视图长度
     * @return 字符串视图长度
     */
    usize length() const {
        return end_ - begin_;
    }

    /**
     * @brief 字符串切片索引访问操作符
     * @param idx 索引位置
     * @return 索引位置的字符引用
     */
    char& operator[](const usize idx) {
        return *(begin_ + idx);
    }

    /**
     * @brief 字符串切片索引访问操作符（常量版本）
     * @param idx 索引位置
     * @return 索引位置的字符常量引用
     */
    const char& operator[](const usize idx) const {
        return *(begin_ + idx);
    }

    /**
     * @brief 转化为CString，会拷贝
     * @return 新字符串
     */
    template <typename Alloc = Allocator<char>>
    [[nodiscard]] BasicCString<Alloc> to_string() const {
        return {begin_, length()};
    }

    /**
     * @brief 与字符串判断相等，不会拷贝
     */
    template <typename Alloc = Allocator<char>>
    friend bool operator==(const Self& view, const BasicCString<Alloc>& cstr) {
        if (view.length() != cstr.length()) {
            return false;
        }

        usize idx = 0;
        for (const char c : cstr) {
            if (view[idx] != c) {
                return false;
            }
            ++idx;
        }
        return true;
    }

    /**
     * @brief 与字符串判断相等，不会拷贝
     */
    template <typename Alloc = Allocator<char>>
    friend bool operator==(const BasicCString<Alloc>& cstr, const Self& view) {
        if (view.length() != cstr.length()) {
            return false;
        }

        usize idx = 0;
        for (const char c : cstr) {
            if (view[idx] != c) {
                return false;
            }
            ++idx;
        }
        return true;
    }

    /**
     * @brief 迭代器支持
     * @tparam IsConst 是否为常量迭代器
     */
    template <bool IsConst>
    class Iterator {
    public:
        using Self = Iterator<IsConst>;
        using iterator_category = std::random_access_iterator_tag;
        using value_type = std::conditional_t<IsConst, const char, char>;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type*;
        using reference = value_type&;

        /**
         * @brief 使用指针构造
         * @param ptr 指针
         */
        explicit Iterator(pointer ptr) :
                ptr_(ptr) {}

        /**
         * @brief 解引用
         */
        reference operator*() const { return *ptr_; }
        pointer operator->() const { return ptr_; }

        /**
         * @brief 前缀递增/递减
         */
        Self& operator++() {
            ++ptr_;
            return *this;
        }

        Self& operator--() {
            --ptr_;
            return *this;
        }

        /**
         * @breif 后缀递增/递减
         */
        Self operator++(int) {
            Self tmp = *this;
            ++*this;
            return tmp;
        }

        Self operator--(int) {
            Self tmp = *this;
            --*this;
            return tmp;
        }

        /**
         * @breif 随机访问
         */
        Self operator+(difference_type n) const { return Self(ptr_ + n); }
        Self operator-(difference_type n) const { return Self(ptr_ - n); }
        difference_type operator-(const Self& other) const { return ptr_ - other.ptr_; }

        /**
         * @brief 复合赋值
         */
        Self& operator+=(difference_type n) {
            ptr_ += n;
            return *this;
        }

        Self& operator-=(difference_type n) {
            ptr_ -= n;
            return *this;
        }

        /**
         * @brief 下标访问
         */
        reference operator[](difference_type n) const { return ptr_[n]; }

        /**
         * @brief 比较运算符
         */
        bool operator==(const Self& other) const { return ptr_ == other.ptr_; }
        bool operator!=(const Self& other) const { return ptr_ != other.ptr_; }
        bool operator<(const Self& other) const { return ptr_ < other.ptr_; }
        bool operator>(const Self& other) const { return ptr_ > other.ptr_; }
        bool operator<=(const Self& other) const { return ptr_ <= other.ptr_; }
        bool operator>=(const Self& other) const { return ptr_ >= other.ptr_; }

    private:
        pointer ptr_;
    };

    using iterator = Iterator<false>;
    using const_iterator = Iterator<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    /**
     * @brief 迭代器接口
     */
    iterator begin() { return iterator(begin_); }
    iterator end() { return iterator(end_); }
    const_iterator begin() const { return const_iterator(begin_); }
    const_iterator end() const { return const_iterator(end_); }
    const_iterator cbegin() const { return begin(); }
    const_iterator cend() const { return end(); }

    reverse_iterator rbegin() { return reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
    const_reverse_iterator crbegin() const { return rbegin(); }
    const_reverse_iterator crend() const { return rend(); }

private:
    char* begin_; // 指向切片首地址
    char* end_;   // 指向切片尾后地址
};

/**
 * @class BasicCString
 * @brief C风格字符串
 * @details 自定义字符串类，提供安全的字符串操作和内存管理
 * @tparam Alloc 内存分配器
 */
template <typename Alloc = Allocator<char>>
class BasicCString {
public:
    using Self = BasicCString<Alloc>;

    /**
     * @brief 根据指定长度创建字符串，填充0
     * @details 实际长度为指定长度+1，尾元为'\0'
     * @param len 字符串的长度
     */
    BasicCString(const usize len = 1) :
            str_(alloc_.allocate(len + 1)), len_(len) {
        std::memset(str_, 0, len + 1);
        str_[len_] = '\0';
    }

    /**
     * @brief 根据字符指针创建字符串
     * @param str 字符指针
     */
    BasicCString(const char* str) :
            Self(str, std::strlen(str)) {}

    /**
     * @brief 根据字符指针和指定长度创建字符串
     * @param str 字符指针
     * @param len 字符串的长度
     */
    BasicCString(const char* str, usize len) :
            Self(len) {
        std::memcpy(data(), str, len);
    }

    /**
     * @brief 根据 std::basic_string 创建字符串
     * @param str std::basic_string 对象
     */
    BasicCString(const std::basic_string<char>& str) :
            Self(str.c_str(), str.size()) {}

    /**
     * @brief 析构函数，释放动态分配的内存
     */
    ~BasicCString() {
        alloc_.deallocate(str_, len_ + 1);
    }

    /**
     * @brief 拷贝构造函数
     * @param other 要拷贝的 CString 对象
     */
    BasicCString(const Self& other) :
            Self(other.data(), other.size()) {}

    /**
     * @brief 移动构造函数
     * @param other 要移动的 CString 对象
     */
    BasicCString(Self&& other) noexcept :
            alloc_(std::move(other.alloc_)), str_(other.str_), len_(other.len_) {
        other.str_ = nullptr;
        other.len_ = 0;
    }

    /**
     * @brief 拷贝赋值操作符
     * @param other 要赋值的 CString 对象
     * @return 自身的引用
     */
    Self& operator=(const Self& other) {
        if (this == &other) return *this;

        char* new_str = alloc_.allocate(other.len_ + 1);
        std::memcpy(new_str, other.str_, other.len_ + 1);
        alloc_.deallocate(str_, len_ + 1);

        str_ = new_str;
        len_ = other.len_;

        return *this;
    }

    /**
     * @brief 移动赋值操作符
     * @param other 要赋值的 CString 对象
     * @return 自身的引用
     */
    Self& operator=(Self&& other) noexcept {
        if (this == &other) return *this;

        alloc_.deallocate(str_, len_ + 1);
        str_ = other.str_;
        len_ = other.len_;
        alloc_ = std::move(other.alloc_);

        other.str_ = nullptr;
        other.len_ = 0;

        return *this;
    }

    /**
     * @brief 创建单字符字符串
     */
    [[nodiscard]] static Self of(char ch) {
        Self str(1);
        str.str_[0] = ch;
        return str;
    }

    /**
     * @brief 创建长度为size的字符串，填充字符ch
     */
    [[nodiscard]] static Self of(usize size, char ch) {
        Self str(size);
        for (usize i = 0; i < size; ++i) {
            str[i] = ch;
        }
        return str;
    }

    /**
     * @brief 隐式转换为 const char* 类型
     * @return 字符串的 const char* 表示
     */
    operator const char*() const {
        return data();
    }

    /**
     * @brief 隐式转换为 char* 类型
     * @return 字符串的 char* 表示
     */
    operator char*() {
        return data();
    }

    /**
     * @brief 隐式转换为 boolean 类型（非空为 true）
     * @return 字符串是否非空
     */
    operator bool() {
        return size() != 0;
    }

    /**
     * @brief 字符串索引访问操作符
     * @param idx 索引位置
     * @return 索引位置的字符引用
     */
    char& operator[](const usize idx) {
        return str_[idx];
    }

    /**
     * @brief 字符串索引访问操作符（常量版本）
     * @param idx 索引位置
     * @return 索引位置的字符常量引用
     */
    const char& operator[](const usize idx) const {
        return str_[idx];
    }

    /**
     * @brief 获取字符串的长度，适配可迭代约束
     * @return 字符串的长度
     */
    usize size() const {
        return len_;
    }

    /**
     * @brief 获取字符串的长度
     * @return 字符串的长度
     */
    usize length() const {
        return len_;
    }

    /**
     * @brief 判断字符串是否为空
     * @return true=是 false=否
     */
    bool empty() const {
        return len_ == 0;
    }

    /**
     * @brief 获取字符串的起始地址
     * @return 字符串的起始地址
     */
    char* data() {
        return str_;
    }

    /**
     * @brief 获取字符串的起始地址（常量版本）
     * @return 字符串的起始地址
     */
    const char* data() const {
        return str_;
    }

    /**
     * @brief 字符串切片，返回指定范围的子字符串
     * @param start 起始索引
     * @param end 结束索引（不包含）
     * @return 子字符串
     */
    CStringView slice(const usize start, isize end) const {
        end = neg_index(end, static_cast<isize>(length()));
        return CStringView{str_ + start, end - start};
    }

    /**
     * @brief 字符串切片，返回从指定索引开始到末尾的子字符串
     * @param start 起始索引
     * @return 子字符串
     */
    CStringView slice(const usize start) const {
        return slice(start, length());
    }

    /**
     * @brief 查找指定字符，找不到返回npos
     */
    usize find(const char ch) const {
        const auto m_size = length();
        for (usize i = 0; i < m_size; ++i) {
            if (str_[i] == ch) {
                return i;
            }
        }
        return npos;
    }

    /**
     * @brief 查找第一个不匹配的位置，若全部匹配，返回npos
     */
    usize find_first_not_of(const char ch) const {
        const auto m_size = length();
        for (usize i = 0; i < m_size; ++i) {
            if (str_[i] != ch) {
                return i;
            }
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
        if (pattern.empty()) return npos;
        const auto m_size = size(), p_size = pattern.size();
        const auto next = get_next(pattern);
        for (usize i = pos, j = 0; i < m_size; ++i) {
            // 失配，j按照next回跳
            while (j > 0 && (*this)[i] != pattern[j]) {
                j = next[j - 1];
            }
            j += (*this)[i] == pattern[j]; // 匹配，j前进
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
    std::vector<usize> find_all(const Self& pattern) const {
        std::vector<usize> res;
        if (pattern.empty()) return res;
        const auto m_size = size(), p_size = pattern.size();
        const auto next = get_next(pattern);
        for (usize i = 0, j = 0; i < m_size; ++i) {
            // 失配，j按照next回跳
            while (j > 0 && (*this)[i] != pattern[j]) {
                j = next[j - 1];
            }
            j += (*this)[i] == pattern[j]; // 匹配，j前进
            // 模式串匹配完，收集文本串匹配起点
            if (j == p_size) {
                res.push_back(i - p_size + 1);
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
     * @brief 将字符串转换为全大写
     * @return 全大写的字符串
     */
    Self upper() const {
        Self res{*this};
        const auto m_size = length();
        for (usize i = 0; i < m_size; ++i) {
            res[i] = std::toupper(res[i]);
        }
        return res;
    }

    /**
     * @brief 将字符串转换为全小写
     * @return 全小写的字符串
     */
    Self lower() const {
        Self res{*this};
        const auto m_size = length();
        for (usize i = 0; i < m_size; ++i) {
            res[i] = std::tolower(res[i]);
        }
        return res;
    }

    /**
     * @brief 去除字符串首尾的空白字符
     * @return 去除空白后的字符串
     */
    CStringView trim() const {
        auto [l, r] = get_trim_index();
        return slice(l, r);
    }

    /**
     * @brief 去除字符串首部的空白字符
     * @return 去除首部空白后的字符串
     */
    CStringView ltrim() const {
        return slice(get_ltrim_index());
    }

    /**
     * @brief 去除字符串尾部的空白字符
     * @return 去除尾部空白后的字符串
     */
    CStringView rtrim() const {
        return slice(get_rtrim_index());
    }

    /**
     * @brief 去除字符串首尾的指定模式
     * @param pattern 要去除的模式
     * @return 去除模式后的字符串
     */
    CStringView trim(const Self& pattern) const {
        auto [l, r] = get_trim_index(pattern);
        return slice(l, r);
    }

    /**
     * @brief 去除字符串首部的指定模式
     * @param pattern 要去除的模式
     * @return 去除模式后的字符串
     */
    CStringView ltrim(const Self& pattern) const {
        return slice(get_ltrim_index(pattern));
    }

    /**
     * @brief 去除字符串尾部的指定模式
     * @param pattern 要去除的模式
     * @return 去除模式后的字符串
     */
    CStringView rtrim(const Self& pattern) const {
        return slice(get_rtrim_index(pattern));
    }

    /**
     * @brief 删除字符串中所有指定字符
     * @param ch 要删除的字符
     * @return 新字符串
     */
    Self remove_all(const char ch) const {
        usize new_len = 0;
        for (usize i = 0; i < len_; ++i) {
            if (str_[i] != ch) {
                new_len++;
            }
        }

        Self res(new_len);
        usize pos = 0;
        for (usize i = 0; i < len_; ++i) {
            if (str_[i] != ch) {
                res[pos++] = str_[i];
            }
        }
        return res;
    }

    /**
     * @brief 删除字符串中所有满足谓词的字符
     * @param pred 谓词
     * @return 新字符串
     */
    Self remove_all(Pred<char>&& pred) const {
        usize new_len = 0;
        for (usize i = 0; i < len_; ++i) {
            if (!pred(str_[i])) {
                new_len++;
            }
        }

        Self res(new_len);
        usize pos = 0;
        for (usize i = 0; i < len_; ++i) {
            if (!pred(str_[i])) {
                res[pos++] = str_[i];
            }
        }
        return res;
    }

    /**
     * @brief 字符串拼接操作符
     * @param other 要拼接的 CString 对象
     * @return 拼接后的新字符串
     */
    Self operator+(const Self& other) const {
        Self res(len_ + other.len_);
        std::memcpy(res.str_, str_, len_);
        std::memcpy(res.str_ + len_, other.str_, other.len_ + 1);
        return res;
    }

    /**
     * @brief 字符串拼接操作符
     * @param str 要拼接的字符串
     * @return 拼接后的新字符串
     */
    Self operator+(const char* str) const {
        return *this + Self{str};
    }

    /**
     * @brief 转为字符串
     * @return 自身对象
     */
    [[nodiscard]] Self __str__() const {
        return *this;
    }

    /**
     * @brief 计算字符串的哈希值
     * @return 字符串的哈希值
     */
    [[nodiscard]] hash_t __hash__() const {
        return bytes_hash(data(), size());
    }

    /**
     * @brief 比较两个字符串
     * @param other 要比较的字符串
     * @return 比较结果
     */
    [[nodiscard]] cmp_t __cmp__(const Self& other) const {
        return std::strcmp(data(), other.data());
    }

    /**
     * @brief 判断两个字符串是否相等
     * @param other 要比较的字符串
     * @return 是否相等
     */
    [[nodiscard]] bool __equals__(const Self& other) const {
        return __cmp__(other) == 0;
    }

    /**
     * @brief 判断字符串与 C 风格字符串是否相等
     * @param other 要比较的 C 风格字符串
     * @return 是否相等
     */
    [[nodiscard]] bool __equals__(const char* other) const {
        return std::strcmp(data(), other) == 0;
    }

    /**
     * @brief 比较运算符
     */
    bool operator>(const Self& other) const { return __cmp__(other) > 0; }
    bool operator<(const Self& other) const { return __cmp__(other) < 0; }
    bool operator>=(const Self& other) const { return __cmp__(other) >= 0; }
    bool operator<=(const Self& other) const { return __cmp__(other) <= 0; }
    bool operator==(const Self& other) const { return __cmp__(other) == 0; }
    bool operator!=(const Self& other) const { return __cmp__(other) != 0; }
    bool operator==(const char* other) const { return __equals__(other); }
    bool operator!=(const char* other) const { return !__equals__(other); }

    /**
     * @brief 迭代器支持
     * @tparam IsConst 是否为常量迭代器
     */
    template <bool IsConst>
    class Iterator {
    public:
        using Self = Iterator<IsConst>;
        using iterator_category = std::random_access_iterator_tag;
        using value_type = std::conditional_t<IsConst, const char, char>;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;

        /**
         * @brief 使用指针构造
         * @param curr 当前位置指针
         */
        explicit Iterator(pointer curr) :
                curr_(curr) {}

        Iterator(const Self& other) = default;
        Self& operator=(const Self& other) = default;

        Iterator(Self&&) = default;
        Self& operator=(Self&&) = default;

        ~Iterator() = default;

        /**
         * @brief 解引用
         */
        reference operator*() { return *curr_; }
        const_reference operator*() const { return *curr_; }
        pointer operator->() { return curr_; }
        const_pointer operator->() const { return curr_; }

        /**
         * @breif 前缀递增/递减
         */
        Self& operator++() {
            ++curr_;
            return *this;
        }

        Self& operator--() {
            --curr_;
            return *this;
        }

        /**
         * @breif 后缀递增/递减
         */
        Self operator++(i32) {
            Self tmp(*this);
            ++*this;
            return tmp;
        }

        Self operator--(i32) {
            Self tmp(*this);
            --*this;
            return tmp;
        }

        /**
         * @breif 随机访问
         */
        Self operator+(difference_type n) const { return Self(curr_ + n); }
        Self operator-(difference_type n) const { return Self(curr_ - n); }
        difference_type operator-(const Self& other) const { return curr_ - other.curr_; }

        /**
         * @brief 复合赋值
         */
        Self& operator+=(difference_type n) {
            curr_ += n;
            return *this;
        }

        Self& operator-=(difference_type n) {
            curr_ -= n;
            return *this;
        }

        /**
         * @brief 下标访问
         */
        reference operator[](difference_type n) const { return curr_[n]; }

        /**
         * @brief 比较运算符
         */
        bool operator==(const Self& other) const { return curr_ == other.curr_; }
        bool operator!=(const Self& other) const { return curr_ != other.curr_; }
        bool operator<(const Self& other) const { return curr_ < other.curr_; }
        bool operator>(const Self& other) const { return curr_ > other.curr_; }
        bool operator<=(const Self& other) const { return curr_ <= other.curr_; }
        bool operator>=(const Self& other) const { return curr_ >= other.curr_; }

    private:
        pointer curr_;
    };

    using iterator = Iterator<false>;
    using const_iterator = Iterator<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    /**
     * @brief 迭代器接口
     */
    iterator begin() { return iterator(str_); }
    iterator end() { return iterator(str_ + len_); }
    const_iterator begin() const { return const_iterator(str_); }
    const_iterator end() const { return const_iterator(str_ + len_); }
    const_iterator cbegin() const { return begin(); }
    const_iterator cend() const { return end(); }

    reverse_iterator rbegin() { return reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
    const_reverse_iterator crbegin() const { return rbegin(); }
    const_reverse_iterator crend() const { return rend(); }

private:
    /**
     * @brief KMP辅助函数，求next数组
     * @param pattern 模式串
     * @note next[i]: 模式串[0, i)中最长相等前后缀的长度为next[i]
     * @note 时间复杂度为 O(m)，m为模式串的长度
     */
    static std::vector<usize> get_next(const Self& pattern) {
        const auto p_size = pattern.size();
        std::vector<usize> next(p_size, 0);
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

    /**
     * @brief 获取去除首尾空白后的索引范围
     * @return 首尾空白后的索引范围
     */
    std::pair<usize, usize> get_trim_index() const {
        usize l = 0, r = length();
        while (l < r && str_[l] == ' ') ++l;
        while (l < r && str_[r - 1] == ' ') --r;
        return std::make_pair(l, r);
    }

    /**
     * @brief 获取去除首尾模式后的索引范围
     * @param pattern 要去除的模式
     * @return 首尾模式后的索引范围
     */
    std::pair<usize, usize> get_trim_index(const Self& pattern) const {
        usize l = 0, r = length();
        const auto p_size = pattern.length();
        while (l + p_size <= r && slice(l, l + p_size) == pattern) l += p_size;
        while (l + p_size <= r && slice(r - p_size, r) == pattern) r -= p_size;
        return std::make_pair(l, r);
    }

    /**
     * @brief 获取去除首部空白后的索引
     * @return 去除首部空白后的索引
     */
    usize get_ltrim_index() const {
        usize l = 0;
        const auto r = length();
        while (l < r && str_[l] == ' ') ++l;
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
        while (l < r && str_[r - 1] == ' ') --r;
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
    Alloc alloc_{}; // 内存分配器
    char* str_;     // 存储字符串的动态数组
    usize len_;     // 字符串的长度
};

/**
 * @brief CString 类，使用默认分配器
 */
using CString = BasicCString<Allocator<char>>;

/**
 * @brief 根据不同类型转换为 CString 对象（适用于自定义可打印类型）
 * @tparam T 自定义可打印类型
 * @param value 要转换的值
 * @return 转换后的 CString 对象
 */
template <MyPrintable T>
fn cstr_impl(const T& value) -> CString {
    return value.__str__();
}

/**
 * @brief 根据不同类型转换为 CString 对象（适用于标准可打印类型）
 * @tparam T 标准可打印类型
 * @param value 要转换的值
 * @return 转换后的 CString 对象
 */
template <StdPrintable T>
fn cstr_impl(const T& value) -> CString {
    std::stringstream stream;
    stream << value;
    return stream.str();
}

/**
 * @brief 根据不同类型自动选择合适的转换方法生成 CString 对象
 * @tparam T 可打印类型
 * @param value 要转换的值
 * @return 转换后的 CString 对象
 */
template <Printable T>
fn cstr(const T& value) -> CString {
    return cstr_impl(value);
}

/**
 * @brief 将 CString 对象转换为标准 C 风格字符串
 * @param value CString 对象
 * @return 标准 C 风格字符串
 */
fn stdstr(const CString& value) -> const char* {
    return value.data();
}

/**
 * @brief 将字符转换为整数（仅适用于 ASCII 数字字符）
 * @param ch 字符
 * @return 对应的整数值
 */
fn c2i(const char ch) -> i32 {
    return ch - '0';
}

/**
 * @brief 将整数转换为对应字符（仅适用于 0-9 数字）
 * @param ch 整数
 * @return 对应的字符
 */
fn i2c(const i32 ch) -> char {
    return ch + '0';
}

/**
 * @brief 自定义字符串字面量，支持 `_cs` 后缀转换为 CString 对象
 * @param str C 风格字符串
 * @param len 字符串长度
 * @return 转换后的 CString 对象
 */
fn operator""_cs(const char* str, const size_t len)->CString {
    return CString{str, len};
}

} // namespace my

/**
 * @brief 为 CString 类提供标准库格式化支持
 */
template <typename Alloc>
struct std::formatter<my::BasicCString<Alloc>> : std::formatter<const char*> {
    fn format(const my::BasicCString<Alloc>& value, auto& ctx) const {
        return std::formatter<const char*>::format(value.data(), ctx);
    }
};

#endif // CSTRING_HPP