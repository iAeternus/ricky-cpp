/**
 * @brief 自定义字符串类，支持 Unicode 编码
 * @author Ricky
 * @date 2024/12/19
 * @version 1.0
 */
#ifndef STRING_HPP
#define STRING_HPP

#include "CodePoint.hpp"
#include "Encoding.hpp"

namespace my::util {

/**
 * @brief 字符串，支持 Unicode 编码和多种操作
 */
template <typename Alloc = Allocator<CodePoint>>
class BaseString : public Sequence<BaseString<Alloc>, CodePoint> {
public:
    using Self = BaseString<Alloc>;
    using Super = Sequence<Self, CodePoint>;

    static constexpr usize SSO_MAX_SIZE = 16; // SSO最大码点数
    // static constexpr usize SSO_BUFFER_MAX_SIZE = SSO_MAX_SIZE * sizeof(CodePoint); // SSO缓冲区大小

    /**
     * @brief 默认构造函数，创建一个空字符串，并指定编码
     * @param enc 字符串的编码（可选）
     */
    BaseString(const EncodingType enc = EncodingType::UTF8) :
            length_(0), is_sso_(true), encoding_(encoding_map(enc)), code_points_(sso_) {}

    /**
     * @brief 构造函数，使用码点数组和编码创建字符串
     * @param code_points 码点数组
     * @param length 码点数量
     * @param encoding 编码
     */
    BaseString(CodePoint* code_points, usize length, Encoding* encoding) :
            length_(length), is_sso_(length <= SSO_MAX_SIZE), encoding_(encoding) {
        if (is_sso_) {
            for (usize i = 0; i < length_; ++i) {
                alloc_.construct(sso_ + i, code_points[i]);
            }
            code_points_ = sso_;
        } else {
            heap_ = code_points;
            code_points_ = heap_;
        }
    }

    /**
     * @brief 构造函数，根据 C 风格字符串创建字符串
     * @param str C 风格字符串
     * @param length 字符串的长度（可选）
     * @param enc 字符串的编码（可选）
     */
    BaseString(const char* str, usize length = npos, const EncodingType enc = EncodingType::UTF8) {
        Encoding* encoding = encoding_map(enc);
        length = ifelse(length != npos, length, std::strlen(str));
        auto [size, arr] = get_code_points(str, length, encoding).separate();
        length_ = size;
        is_sso_ = length_ <= SSO_MAX_SIZE;
        encoding_ = encoding;
        if (is_sso_) {
            for (usize i = 0; i < length_; ++i) {
                alloc_.construct(sso_ + i, arr[i]);
            }
            code_points_ = sso_;
        } else {
            heap_ = arr;
            code_points_ = heap_;
        }
    }

    /**
     * @brief 构造函数，根据自定义字符串创建字符串
     * @param cstr 自定义字符串
     * @param enc 字符串的编码（可选）
     */
    BaseString(const CString& cstr, EncodingType enc = EncodingType::UTF8) :
            BaseString(cstr.data(), cstr.size(), enc) {}

    /**
     * @brief 构造函数，根据码点创建字符串
     * @param cp 码点
     * @param enc 字符串的编码（可选）
     */
    BaseString(const CodePoint& cp, EncodingType enc = EncodingType::UTF8) :
            BaseString(cp.data(), cp.size(), enc) {}

    /**
     * @brief 构造函数，创建一个长度为 length 的字符串，并用码点 cp 填充
     * @param length 字符串的长度
     * @param cp 码点，用于填充整个字符串
     * @param enc 字符串的编码
     */
    BaseString(usize length, const CodePoint& cp = ' ', const EncodingType enc = EncodingType::UTF8) :
            length_(length), is_sso_(length <= SSO_MAX_SIZE), encoding_(encoding_map(enc)) {
        if (is_sso_) {
            for (usize i = 0; i < length_; ++i) {
                alloc_.construct(sso_ + i, cp);
            }
            code_points_ = sso_;
        } else {
            heap_ = alloc_.allocate(length_);
            for (usize i = 0; i < length_; ++i) {
                alloc_.construct(heap_ + i, cp);
            }
            code_points_ = heap_;
        }
    }

    /**
     * @brief 拷贝构造函数
     * @param other 要拷贝的字符串对象
     */
    BaseString(const Self& other) :
            length_(other.length_), is_sso_(other.is_sso_), encoding_(other.encoding_) {
        if (is_sso_) {
            for (usize i = 0; i < length_; ++i) {
                alloc_.construct(sso_ + i, other.code_points_[i]);
            }
            code_points_ = sso_;
        } else {
            heap_ = alloc_.allocate(length_);
            for (usize i = 0; i < length_; ++i) {
                alloc_.construct(heap_ + i, other.code_points_[i]);
            }
            code_points_ = heap_;
        }
    }

    /**
     * @brief 移动构造函数
     * @param other 要移动的字符串对象
     */
    BaseString(Self&& other) noexcept :
            length_(other.length_), is_sso_(other.is_sso_), encoding_(other.encoding_) {
        if (is_sso_) {
            for (usize i = 0; i < length_; ++i) {
                alloc_.construct(sso_ + i, std::move(other.code_points_[i]));
            }
            code_points_ = sso_;
        } else {
            heap_ = other.heap_;
            code_points_ = heap_;
        }
        other.length_ = 0;
        other.is_sso_ = true;
        other.encoding_ = nullptr;
        other.code_points_ = other.sso_; // TODO 联合是否能修改两个成员？
        other.heap_ = nullptr;
    }

    /**
     * @brief 析构函数，自动释放堆分配内存
     */
    ~BaseString() {
        if (!is_sso_ && heap_ != nullptr) {
            alloc_.destroy(heap_, length_);
            alloc_.deallocate(heap_, length_);
            heap_ = nullptr;
        }
        length_ = 0;
        is_sso_ = true;
        encoding_ = nullptr;
        code_points_ = nullptr;
    }

    /**
     * @brief 拷贝赋值操作符
     * @param other 要赋值的字符串对象
     * @return 自身的引用
     */
    Self& operator=(const Self& other) {
        if (this == &other) return *this;
        if (!is_sso_ && heap_ != nullptr) {
            alloc_.destroy(heap_, length_);
            alloc_.deallocate(heap_, length_);
            heap_ = nullptr;
        }
        this->length_ = other.length_;
        this->is_sso_ = other.is_sso_;
        this->encoding_ = other.encoding_;
        if (is_sso_) {
            for (usize i = 0; i < length_; ++i) {
                alloc_.construct(sso_ + i, other.code_points_[i]);
            }
            code_points_ = sso_;
        } else {
            heap_ = alloc_.allocate(length_);
            for (usize i = 0; i < length_; ++i) {
                alloc_.construct(heap_ + i, other.code_points_[i]);
            }
            code_points_ = heap_;
        }
        return *this;
    }

    /**
     * @brief 移动赋值操作符
     * @param other 要赋值的字符串对象
     * @return 自身的引用
     */
    Self& operator=(Self&& other) noexcept {
        if (this == &other) return *this;
        if (!is_sso_ && heap_ != nullptr) {
            alloc_.destroy(heap_, length_);
            alloc_.deallocate(heap_, length_);
            heap_ = nullptr;
        }
        this->length_ = other.length_;
        this->is_sso_ = other.is_sso_;
        this->encoding_ = other.encoding_;
        if (is_sso_) {
            for (usize i = 0; i < length_; ++i) {
                alloc_.construct(sso_ + i, std::move(other.code_points_[i]));
            }
            code_points_ = sso_;
        } else {
            heap_ = other.heap_;
            code_points_ = heap_;
            other.heap_ = nullptr;
        }
        other.length_ = 0;
        other.is_sso_ = true;
        other.encoding_ = nullptr;
        other.code_points_ = other.sso_;
        return *this;
    }

    /**
     * @brief 从i32构造
     */
    static Self from_i32(i32 val) {
        char buf[16];
        int len = std::snprintf(buf, sizeof(buf), "%d", val);
        return Self(buf, static_cast<usize>(len), EncodingType::UTF8);
    }

    /**
     * @brief 从u32构造
     */
    static Self from_u32(u32 val) {
        char buf[16];
        int len = std::snprintf(buf, sizeof(buf), "%u", val);
        return Self(buf, static_cast<usize>(len), EncodingType::UTF8);
    }

    /**
     * @brief 从i64构造
     */
    static Self from_i64(i64 val) {
        char buf[32];
        int len = std::snprintf(buf, sizeof(buf), "%lld", val);
        return Self(buf, static_cast<usize>(len), EncodingType::UTF8);
    }

    /**
     * @brief 从u64构造
     */
    static Self from_u64(u64 val) {
        char buf[32];
        int len = std::snprintf(buf, sizeof(buf), "%llu", val);
        return Self(buf, static_cast<usize>(len), EncodingType::UTF8);
    }

    /**
     * @brief 从f64构造，会自动去除多余的0
     */
    static Self from_f64(f64 val) {
        char buf[32];
        int len = std::snprintf(buf, sizeof(buf), "%g", val);
        return Self(buf, static_cast<usize>(len), EncodingType::UTF8);
    }

    /**
     * @brief 转换为std::string
     */
    std::string into_string() const {
        std::string result;
        result.reserve(byte_length());
        for (usize i = 0; i < length_; ++i) {
            const CodePoint& cp = code_points_[i];
            result.append(cp.data(), cp.size());
        }
        return result;
    }

    /**
     * @brief 字符串拼接操作符
     * @param other 要拼接的字符串对象
     * @return 拼接后的新字符串
     */
    Self operator+(const Self& other) const {
        usize m_size = this->size(), o_size = other.size();
        Self res{m_size + o_size, this->encoding()};
        for (usize i = 0; i < m_size; ++i) {
            res[i] = (*this)[i];
        }
        for (usize i = 0; i < o_size; ++i) {
            res[m_size + i] = other[i];
        }
        return res;
    }

    /**
     * @brief 字符串拼接操作符
     * @param other 要拼接的C风格字符串对象
     * @return 拼接后的新字符串
     */
    Self operator+(const CString& other) const {
        usize m_size = this->size(), o_size = other.size();
        Self res{m_size + o_size, this->encoding()};
        for (usize i = 0; i < m_size; ++i) {
            res[i] = (*this)[i];
        }
        for (usize i = 0; i < o_size; ++i) {
            res[m_size + i] = *CodePointPool::instance().get(other[i]);
        }
        return res;
    }

    /**
     * @brief 累加字符串拼接操作符
     * @param other 要拼接的字符串对象
     * @return 自身的引用
     */
    Self& operator+=(const Self& other) {
        Self res = *this + other;
        *this = std::move(res);
        return *this;
    }

    /**
     * @brief 累加字符串拼接操作符
     * @param other 要拼接的C风格字符串对象
     * @return 自身的引用
     */
    Self& operator+=(const CString& other) {
        Self res = *this + other;
        *this = std::move(res);
        return *this;
    }

    /**
     * @brief 字符串复制操作符
     * @param n 复制的份数
     * @return 复制后的新字符串
     */
    Self operator*(usize n) {
        usize pos = 0, m_size = size();
        Self res{m_size * n, encoding()};
        while (n--) {
            for (usize i = 0; i < m_size; ++i) {
                res[pos++] = (*this)[i];
            }
        }
        return res;
    }

    /**
     * @brief 获取指定索引处的码点
     * @param index 索引位置
     * @return 索引位置的码点
     * @exception index_out_of_bounds_exception 若下标越界
     */
    CodePoint& at(usize index) {
        if (index > length_) {
            throw index_out_of_bounds_exception("index {} out of bounds [0..{}]", SRC_LOC, index, length_);
        }
        return code_points_[index];
    }

    /**
     * @brief 获取指定索引处的码点（常量版本）
     * @param index 索引位置
     * @return 索引位置的码点
     * @exception index_out_of_bounds_exception 若下标越界
     */
    const CodePoint& at(usize index) const {
        if (index > length_) {
            throw index_out_of_bounds_exception("index {} out of bounds [0..{}]", SRC_LOC, index, length_);
        }
        return code_points_[index];
    }

    /**
     * @brief 索引操作符，返回指定索引处的码点，不会检查越界
     * @param index 索引位置
     * @return 索引位置的码点
     */
    CodePoint& operator[](usize index) {
        return code_points_[index];
    }

    /**
     * @brief 索引操作符（常量版本），返回指定索引处的码点，不会检查越界
     * @param index 索引位置
     * @return 索引位置的码点
     */
    const CodePoint& operator[](usize index) const {
        return code_points_[index];
    }

    /**
     * @brief 获取字符串的长度
     * @return 字符串的长度
     */
    usize size() const {
        return length_;
    }

    usize length() const {
        return length_;
    }

    /**
     * @brief 判断字符串是否为空
     * @return true=是 false=否
     */
    bool empty() const {
        return length_ == 0;
    }

    /**
     * @brief 获取字符串的编码
     * @return 字符串的编码
     */
    Encoding* encoding() const {
        return encoding_;
    }

    /**
     * @brief 获取字符串的字节长度
     * @return 字符串的字节长度
     */
    usize byte_length() const {
        usize length = 0;
        for (auto&& ch : *this) {
            length += ch.size();
        }
        return length;
    }

    /**
     * @brief 清空字符串
     */
    void clear() {
        if (!is_sso_ && heap_ != nullptr) {
            alloc_.destroy(heap_, length_);
            alloc_.deallocate(heap_, length_);
            heap_ = nullptr;
        }
        length_ = 0;
        is_sso_ = true;
        encoding_ = nullptr;
        code_points_ = sso_;
    }

    /**
     * @brief 字符串切片，返回指定范围的子字符串 TODO StringView类
     * @param start 起始索引
     * @param end 结束索引（不包含）
     * @return 子字符串
     */
    Self slice(usize start, isize end) const {
        const auto m_size = size();
        start = neg_index(start, m_size);
        end = neg_index(end, static_cast<isize>(m_size));
        Vec<CodePoint> buf;
        for (usize i = start; i < end; ++i) {
            buf.append(code_points_[i]);
        }
        auto [size, arr] = buf.separate();
        return Self(arr, end - start, encoding_);
    }

    /**
     * @brief 字符串切片，返回从指定索引开始到末尾的子字符串
     * @param start 起始索引
     * @return 子字符串
     */
    Self slice(usize start) const {
        return slice(start, size());
    }

    /**
     * @brief 查找字符在字符串中的第一个位置
     * @param c 要查找的字符
     * @return 字符的位置，未找到返回 `npos`
     */
    usize find(const CodePoint& c) const {
        return Super::find(c);
    }

    /**
     * @brief 查找模式串的第一个匹配位置
     * @param pattern 模式串，长度为m
     * @param pos 起始查找位置（可选）
     * @return 模式串的第一个匹配位置，未找到返回 `npos`
     * @note KMP算法，时间复杂度 O(n + m)，n为文本串的长度
     */
    usize find(const Self& pattern, usize pos = 0) const {
        if (pattern.empty()) return npos;
        auto m_size = size(), p_size = pattern.size();
        auto next = get_next(pattern);
        for (usize i = pos, j = 0; i < m_size; ++i) {
            // 失配，j按照next回跳
            while (j > 0 && (*this)[i] != pattern[j]) {
                j = next[j - 1];
            }
            j += ((*this)[i] == pattern[j]); // 匹配，j前进
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
    Vec<usize> find_all(const Self& pattern) const {
        Vec<usize> res;
        if (pattern.empty()) return res;
        auto m_size = size(), p_size = pattern.size();
        auto next = get_next(pattern);
        for (usize i = 0, j = 0; i < m_size; ++i) {
            // 失配，j按照next回跳
            while (j > 0 && (*this)[i] != pattern[j]) {
                j = next[j - 1];
            }
            j += ((*this)[i] == pattern[j]); // 匹配，j前进
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
    bool starts_with(const Self& prefix) const {
        if (size() < prefix.size()) {
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
        if (size() < suffix.size()) {
            return false;
        }
        return slice(size() - suffix.size()) == suffix;
    }

    /**
     * @brief 将字符串转换为全大写
     * @return 全大写的字符串
     */
    Self upper() const {
        Self res{*this};
        auto m_size = size();
        for (usize i = 0; i < m_size; ++i) {
            res[i] = res[i].upper();
        }
        return res;
    }

    /**
     * @brief 将字符串转换为全小写
     * @return 全小写的字符串
     */
    Self lower() const {
        Self res{*this};
        auto m_size = size();
        for (usize i = 0; i < m_size; ++i) {
            res[i] = res[i].lower();
        }
        return res;
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
     * @brief 使用当前字符串连接可迭代对象的每个元素
     * @param iter 可迭代对象
     * @return 连接后的字符串
     */
    template <Iterable I>
    Self join(const I& iter) const {
        if (iter.size() == 0) {
            return Self{};
        }

        Vec<CString> elem_strs;
        usize total_len = 0;
        for (auto&& elem : iter) {
            elem_strs.append(cstr(elem));
            total_len += elem_strs.back().size() + length_;
        }
        if (!elem_strs.empty()) {
            total_len -= length_;
        }

        Self result{std::max(0ULL, total_len), encoding()};
        usize pos = 0;

        auto elem_it = elem_strs.begin();
        const auto elem_end = elem_strs.end();
        if (elem_it != elem_end) {
            const auto& elem_str = *elem_it;
            for (char i : elem_str) {
                result[pos++] = i;
            }
            ++elem_it;
        }

        for (; elem_it != elem_end; ++elem_it) {
            for (usize i = 0; i < length_; ++i) {
                result[pos++] = (*this)[i];
            }
            const auto& elem_str = *elem_it;
            for (char i : elem_str) {
                result[pos++] = i;
            }
        }

        return result;
    }

    /**
     * @brief 替换字符串中的子字符串
     * @param old_ 要被替换的子字符串
     * @param new_ 替换的新字符串
     * @return 替换后的新字符串
     */
    Self replace(const Self& old_, const Self& new_) const {
        auto indices = find_all(old_);
        usize m_size = size();
        Self result{m_size + indices.size() * (new_.size() - old_.size()), encoding()};
        for (usize i = 0, j = 0, k = 0; i < m_size; ++i) {
            if (j < indices.size() && i == indices[j]) {
                for (auto&& c : new_) {
                    result.code_points_[k++] = c;
                }
                i += old_.size() - 1;
                ++j;
            } else {
                result.code_points_[k++] = (*this)[i];
            }
        }
        return result;
    }

    /**
     * @brief 查找第一个成对出现的字符，并返回子字符串
     * @param left 左字符
     * @param right 右字符
     * @return 包含两个字符的子字符串
     */
    Self match(const CodePoint& left, const CodePoint& right) const {
        usize l = find(left);
        if (l == npos) {
            return Self{};
        }

        usize match_cnt = 1;
        for (usize r = l + 1, m_size = size(); r < m_size; ++r) {
            if ((*this)[r] == right) {
                --match_cnt;
            } else if ((*this)[r] == left) {
                ++match_cnt;
            }

            if (match_cnt == 0) {
                return slice(l, r + 1);
            }
        }

        throw runtime_exception("unmatched parentheses, too many left parentheses");
    }

    /**
     * @brief 将当前字符串以pattern作为分隔符分割
     * @param pattern 分隔符
     * @param maxsplit 最大分割次数（可选，-1表示无限制）
     * @return 分割后的字符串向量
     */
    Vec<Self> split(const Self& pattern, isize maxsplit = -1) const {
        Vec<Self> res;
        usize start = 0;
        usize split_cnt = 0;
        usize m_size = size(), p_size = pattern.size();
        const usize actual_splits = (maxsplit < 0) ? m_size : std::min(static_cast<usize>(maxsplit), m_size);

        // 空模式处理：按每个字符分割
        if (pattern.empty()) {
            for (usize i = 0; i < actual_splits; ++i) {
                res.append(Self((*this)[i], encoding_));
            }
            if (actual_splits < m_size) {
                res.append(slice(actual_splits));
            }
            return res;
        }

        auto positions = find_all(pattern);
        for (auto pos : positions) {
            if (maxsplit >= 0 && split_cnt >= actual_splits) {
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
     * @brief 删除字符串中所有指定字符
     * @param codePoint 要删除的字符
     * @return 删除后的字符串
     */
    Self remove_all(CodePoint&& codePoint) const {
        auto m_size = size();
        Vec<CodePoint> buf;
        for (usize i = 0; i < m_size; ++i) {
            if ((*this)[i] != codePoint) {
                buf.append((*this)[i]);
            }
        }
        auto length = buf.size();
        auto [size, code_points] = buf.separate();
        return Self(code_points, length, encoding_);
    }

    /**
     * @brief 删除字符串中所有满足谓词的字符
     * @param pred 谓词
     * @return 删除后的字符串
     */
    Self remove_all(Pred<const CodePoint&>&& pred) const {
        auto m_size = size();
        Vec<CodePoint> buf;
        for (usize i = 0; i < m_size; ++i) {
            if (!pred((*this)[i])) {
                buf.append((*this)[i]);
            }
        }
        auto length = buf.size();
        auto [size, code_points] = buf.separate();
        return Self(code_points, length, encoding_);
    }

    /**
     * @brief 交换两个字符串
     */
    void swap(const Self& _) noexcept {
        // TODO
    }

    /**
     * @brief 返回字符串的 C 风格字符串表示
     * @return C 风格字符串
     */
    [[nodiscard]] CString __str__() const {
        CString res{byte_length()};
        usize pos = 0;
        for (auto&& ch : *this) {
            usize ch_size = ch.size();
            std::memcpy(res.data() + pos, ch.data(), ch_size);
            pos += ch_size;
        }
        return res;
    }

    /**
     * @brief 返回字符串的哈希值
     * @return 字符串的哈希值
     */
    [[nodiscard]] hash_t __hash__() const {
        return __str__().__hash__();
    }

    [[nodiscard]] cmp_t __cmp__(const Self& other) const {
        auto min_size = std::min(this->size(), other.size());
        for (usize i = 0; i < min_size; ++i) {
            auto cmp = (*this)[i].__cmp__(other[i]);
            if (cmp != 0) {
                return cmp;
            }
        }
        return static_cast<usize>(this->size() - other.size());
    }

private:
    /**
     * @brief 内部构造函数，用于根据编码创建字符串对象
     * @note 对于SSO，每个位置的码点都会调用默认构造函数初始化
     * @note 对于heap，每个位置不会调用初始化
     * @param length 字符串的长度
     * @param encoding 字符串的编码
     */
    BaseString(usize length, Encoding* encoding) :
            length_(length), is_sso_(length_ <= SSO_MAX_SIZE), encoding_(encoding) {
        if (is_sso_) {
            code_points_ = sso_;
        } else {
            heap_ = alloc_.allocate(length_);
            code_points_ = heap_;
        }
    }

    /**
     * @brief 获取去除首尾空白后的索引范围
     * @return 首尾空白后的索引范围
     */
    Pair<usize, usize> get_trim_index() const {
        usize l = 0, r = size();
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
        usize l = 0, r = size(), p_size = pattern.size();
        while (l + p_size <= r && slice(l, l + p_size) == pattern) l += p_size;
        while (l + p_size <= r && slice(r - p_size, r) == pattern) r -= p_size;
        return {l, r};
    }

    /**
     * @brief 获取去除首部空白后的索引
     * @return 去除首部空白后的索引
     */
    usize get_ltrim_index() const {
        usize l = 0, r = size();
        while (l < r && (*this)[l].is_blank()) ++l;
        return l;
    }

    /**
     * @brief 获取去除首部模式后的索引
     * @param pattern 要去除的模式
     * @return 去除首部模式后的索引
     */
    usize get_ltrim_index(const Self& pattern) const {
        usize l = 0, r = size(), p_size = pattern.size();
        while (l + p_size <= r && slice(l, l + p_size) == pattern) l += p_size;
        return l;
    }

    /**
     * @brief 获取去除尾部空白后的索引
     * @return 去除尾部空白后的索引
     */
    usize get_rtrim_index() const {
        usize l = 0, r = size();
        while (l < r && (*this)[r - 1].is_blank()) --r;
        return r;
    }

    /**
     * @brief 获取去除尾部模式后的索引
     * @param pattern 要去除的模式
     * @return 去除尾部模式后的索引
     */
    usize get_rtrim_index(const Self& pattern) const {
        usize l = 0, r = size(), p_size = pattern.size();
        while (l + p_size <= r && slice(r - p_size, r) == pattern) r -= p_size;
        return r;
    }

    /**
     * @brief KMP辅助函数，求next数组
     * @param pattern 模式串
     * @note next[i]: 模式串[0, i)中最长相等前后缀的长度为next[i]
     * @note 时间复杂度为 O(m)，m为模式串的长度
     */
    static Vec<usize> get_next(const Self& pattern) {
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
    Alloc alloc_{};          // 内存分配器
    usize length_;           // 字符串的长度
    bool is_sso_;            // 当前是否为小字符串
    Encoding* encoding_;     // 字符串编码信息
    CodePoint* code_points_; // 指向码点数组（可能指向SSO或堆分配）
    union {
        CodePoint sso_[SSO_MAX_SIZE]; // SSO小字符串存储
        CodePoint* heap_{};           // 堆分配大字符串存储
    };
};

/**
 * 对外别名 String
 */
using String = BaseString<Allocator<CodePoint>>;

} // namespace my::util

namespace my {

/**
 * @brief 自定义字符串字面量，支持 `_s` 后缀转换为 `String` 对象
 * @param str C 风格字符串
 * @param length 字符串长度
 * @return 转换后的 `String` 对象
 */
fn operator""_s(const char* str, size_t length)->util::String {
    return util::String{str, length};
}

} // namespace my

template <typename Alloc>
struct std::formatter<my::util::BaseString<Alloc>, char> : std::formatter<string_view, char> {
    auto format(const my::util::BaseString<Alloc>& str, auto& ctx) const {
        auto s = str.into_string();
        return std::formatter<std::string_view, char>::format(
            std::string_view(s.data(), s.length()), ctx);
    }
};

#endif // STRING_HPP