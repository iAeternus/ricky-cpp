/**
 * @brief 自定义字符串类，支持 Unicode 编码
 * @author Ricky
 * @date 2024/12/19
 * @version 1.0
 */
#ifndef STRING_HPP
#define STRING_HPP

#include "StringView.hpp"

namespace my::util {

/**
 * @class String
 * @brief 字符串，支持 Unicode 编码和多种操作
 */
template <typename Alloc = Allocator<CodePoint>>
class BasicString : public Sequence<BasicString<Alloc>, CodePoint> {
public:
    using Self = BasicString<Alloc>;
    using Super = Sequence<Self, CodePoint>;
    using View = BasicStringView<typename Super::const_iterator, Alloc>;

    static constexpr usize SSO_MAX_SIZE = 16; // SSO最大码点数

    /**
     * @brief 默认构造函数，创建一个空字符串，并指定编码
     * @param enc 字符串的编码（可选）
     */
    BasicString(const EncodingType enc = EncodingType::UTF8) :
            length_(0), is_sso_(true), encoding_(encoding_map(enc)), code_points_(sso_) {}

    /**
     * @brief 构造函数，使用码点数组和编码创建字符串
     * @param code_points 码点数组
     * @param length 码点数量
     * @param encoding 编码
     */
    BasicString(const CodePoint* code_points, const usize length, Encoding* encoding) :
            length_(length), is_sso_(length <= SSO_MAX_SIZE), encoding_(encoding) {
        if (is_sso_) {
            for (usize i = 0; i < length_; ++i) {
                alloc_.construct(sso_ + i, code_points[i]);
            }
            code_points_ = sso_;
        } else {
            heap_ = alloc_.allocate(length_);
            for (usize i = 0; i < length_; ++i) {
                alloc_.construct(heap_ + i, code_points[i]);
            }
            code_points_ = heap_;
        }
    }

    /**
     * @brief 构造函数，根据字符指针创建字符串
     * @param str 字符指针
     * @param length 字符串的长度（可选）
     * @param enc 字符串的编码（可选）
     */
    BasicString(const char* str, usize length = npos, const EncodingType enc = EncodingType::UTF8) {
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
     * @brief 构造函数，根据C 风格字符串创建字符串
     * @param cstr C 风格字符串
     * @param enc 字符串的编码（可选）
     */
    BasicString(const CString& cstr, const EncodingType enc = EncodingType::UTF8) :
            BasicString(cstr.data(), cstr.length(), enc) {}

    /**
     * @brief 构造函数，根据码点创建字符串
     * @param cp 码点
     * @param enc 字符串的编码（可选）
     */
    BasicString(const CodePoint& cp, const EncodingType enc = EncodingType::UTF8) :
            BasicString(cp.data(), cp.size(), enc) {}

    /**
     * @brief 构造函数，创建一个长度为 length 的字符串，并用码点 cp 填充
     * @param length 字符串的长度
     * @param cp 码点，用于填充整个字符串
     * @param enc 字符串的编码
     */
    BasicString(const usize length, const CodePoint& cp = ' ', const EncodingType enc = EncodingType::UTF8) :
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

    template <std::input_iterator Iter>
    BasicString(Iter first, Iter last, Encoding* encoding) :
            length_(static_cast<usize>(std::distance(first, last))), is_sso_(length_ <= SSO_MAX_SIZE), encoding_(encoding) {
        if (is_sso_) {
            for (usize i = 0; i < length_; ++i) {
                alloc_.construct(sso_ + i, *first++);
            }
            code_points_ = sso_;
        } else {
            heap_ = alloc_.allocate(length_);
            for (usize i = 0; i < length_; ++i) {
                alloc_.construct(heap_ + i, *first++);
            }
            code_points_ = heap_;
        }
    }

    /**
     * @brief 根据字符串视图构造，
     * @param view
     */
    BasicString(const View& view) :
            Self(view.begin(), view.end(), view.encoding()) {}

    /**
     * @brief 拷贝构造函数
     * @param other 要拷贝的字符串对象
     */
    BasicString(const Self& other) :
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
    BasicString(Self&& other) noexcept :
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
    ~BasicString() {
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
    static Self from_i32(const i32 val) {
        char buf[16];
        const auto len = std::snprintf(buf, sizeof(buf), "%d", val);
        return Self(buf, static_cast<usize>(len), EncodingType::UTF8);
    }

    /**
     * @brief 从u32构造
     */
    static Self from_u32(const u32 val) {
        char buf[16];
        const auto len = std::snprintf(buf, sizeof(buf), "%u", val);
        return Self(buf, static_cast<usize>(len), EncodingType::UTF8);
    }

    /**
     * @brief 从i64构造
     */
    static Self from_i64(const i64 val) {
        char buf[32];
        const auto len = std::snprintf(buf, sizeof(buf), "%lld", val);
        return Self(buf, static_cast<usize>(len), EncodingType::UTF8);
    }

    /**
     * @brief 从u64构造
     */
    static Self from_u64(const u64 val) {
        char buf[32];
        const auto len = std::snprintf(buf, sizeof(buf), "%llu", val);
        return Self(buf, static_cast<usize>(len), EncodingType::UTF8);
    }

    /**
     * @brief 从f64构造，会自动去除多余的0
     */
    static Self from_f64(const f64 val) {
        char buf[32];
        const auto len = std::snprintf(buf, sizeof(buf), "%g", val);
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
    Self operator+(const View& other) const {
        const usize m_size = this->length(), o_size = other.length();
        Self res{m_size + o_size, this->encoding()};
        for (usize i = 0; i < m_size; ++i) {
            res[i] = this->operator[](i);
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
    Self operator+(const CStringView& other) const {
        const usize m_size = this->length(), o_size = other.length();
        Self res{m_size + o_size, this->encoding()};
        for (usize i = 0; i < m_size; ++i) {
            res[i] = this->operator[](i);
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
    Self& operator+=(const View& other) {
        Self res = *this + other;
        *this = std::move(res);
        return *this;
    }

    /**
     * @brief 累加字符串拼接操作符
     * @param other 要拼接的C风格字符串对象
     * @return 自身的引用
     */
    Self& operator+=(const CStringView& other) {
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
        usize pos = 0;
        const auto m_size = length();
        Self res{m_size * n, encoding()};
        while (n--) {
            for (usize i = 0; i < m_size; ++i) {
                res[pos++] = this->operator[](i);
            }
        }
        return res;
    }

    /**
     * @brief 获取指定索引处的码点
     * @param index 索引位置
     * @return 索引位置的码点
     * @exception Exception 若下标越界，则抛出 index_out_of_bounds_exception
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
     * @exception Exception 若下标越界，则抛出 index_out_of_bounds_exception
     */
    const CodePoint& at(const usize index) const {
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
    CodePoint& operator[](const usize index) {
        return code_points_[index];
    }

    /**
     * @brief 索引操作符（常量版本），返回指定索引处的码点，不会检查越界
     * @param index 索引位置
     * @return 索引位置的码点
     */
    const CodePoint& operator[](const usize index) const {
        return code_points_[index];
    }

    /**
     * @brief 获取字符串的长度，适配可迭代约束
     * @return 字符串的长度
     */
    usize size() const {
        return length_;
    }

    /**
     * @brief 获取字符串的长度
     * @return 字符串的长度
     */
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
     * @brief 字符串切片，返回指定范围的子字符串
     * @param start 起始索引
     * @param end 结束索引（不包含）
     * @return 子字符串
     */
    View slice(const usize start, isize end) const {
        end = neg_index(end, static_cast<isize>(length()));
        return View(Super::begin() + start, Super::begin() + static_cast<usize>(end), encoding_->type());
    }

    /**
     * @brief 字符串切片，返回从指定索引开始到末尾的子字符串
     * @param start 起始索引
     * @return 子字符串
     */
    View slice(const usize start) const {
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
    usize find(const View& pattern, const usize pos = 0) const {
        return StringAlgorithm::kmp_find(Super::begin() + pos, Super::end(), pattern.begin(), pattern.end());
    }

    /**
     * @brief 查找模式串的所有匹配位置
     * @param pattern 模式串，长度为m
     * @return 所有匹配位置
     * @note KMP算法，时间复杂度 O(n + m)，n为文本串的长度
     */
    Vec<usize> find_all(const View& pattern) const {
        return StringAlgorithm::kmp_find_all(Super::begin(), Super::end(), pattern.begin(), pattern.end());
    }

    /**
     * @brief 检查字符串是否以指定子字符串开头
     * @param prefix 要检查的子字符串
     * @return 是否以指定子字符串开头
     */
    bool starts_with(const View& prefix) const {
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
    bool ends_with(const View& suffix) const {
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
        const auto m_size = length();
        for (usize i = 0; i < m_size; ++i) {
            res[i] = res[i].lower();
        }
        return res;
    }

    /**
     * @brief 去除字符串首尾的空白字符
     * @return 去除空白后的字符串
     */
    View trim() const {
        auto [l, r] = get_trim_index();
        return slice(l, r);
    }

    /**
     * @brief 去除字符串首部的空白字符
     * @return 去除首部空白后的字符串
     */
    View ltrim() const {
        return slice(get_ltrim_index());
    }

    /**
     * @brief 去除字符串尾部的空白字符
     * @return 去除尾部空白后的字符串
     */
    View rtrim() const {
        return slice(get_rtrim_index());
    }

    /**
     * @brief 去除字符串首尾的指定模式
     * @param pattern 要去除的模式
     * @return 去除模式后的字符串
     */
    View trim(const View& pattern) const {
        auto [l, r] = get_trim_index(pattern);
        return slice(l, r);
    }

    /**
     * @brief 去除字符串首部的指定模式
     * @param pattern 要去除的模式
     * @return 去除模式后的字符串
     */
    View ltrim(const View& pattern) const {
        return slice(get_ltrim_index(pattern));
    }

    /**
     * @brief 去除字符串尾部的指定模式
     * @param pattern 要去除的模式
     * @return 去除模式后的字符串
     */
    View rtrim(const View& pattern) const {
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
            total_len += elem_strs.back().length() + length_;
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
                result[pos++] = this->operator[](i);
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
    Self replace(const View& old_, const View& new_) const {
        const auto indices = find_all(old_);
        const auto m_size = length();
        Self result{m_size + indices.size() * (new_.length() - old_.length()), encoding()};
        for (usize i = 0, j = 0, k = 0; i < m_size; ++i) {
            if (j < indices.size() && i == indices[j]) {
                for (auto&& c : new_) {
                    result.code_points_[k++] = c;
                }
                i += old_.length() - 1;
                ++j;
            } else {
                result.code_points_[k++] = this->operator[](i);
            }
        }
        return result;
    }

    /**
     * @brief 查找第一个成对出现的字符，并返回子字符串
     * @param left 左字符
     * @param right 右字符
     * @return 包含两个字符的子字符串
     * @exception Exception 若字符不匹配，则抛出 runtime_exception
     */
    Self match(const CodePoint& left, const CodePoint& right) const {
        const auto l = find(left);
        if (l == npos) {
            return Self{};
        }

        usize match_cnt = 1;
        for (usize r = l + 1, m_size = length(); r < m_size; ++r) {
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
     * @param max_split 最大分割次数（可选，-1表示无限制）
     * @return 分割后的字符串向量
     */
    Vec<Self> split(const View& pattern, const isize max_split = -1) const {
        Vec<Self> res;
        usize start = 0;
        usize split_cnt = 0;
        const auto m_size = length(), p_size = pattern.length();
        const auto actual_splits = (max_split < 0) ? m_size : std::min(static_cast<usize>(max_split), m_size);

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
     * @brief 删除字符串中所有指定字符
     * @param codePoint 要删除的字符
     * @return 删除后的字符串
     */
    Self remove_all(CodePoint&& codePoint) const {
        return remove_all([&](const auto& cp) {
            return cp == codePoint;
        });
    }

    /**
     * @brief 删除字符串中所有满足谓词的字符
     * @param pred 谓词
     * @return 删除后的字符串
     */
    Self remove_all(Pred<const CodePoint&>&& pred) const {
        const auto m_size = length();
        Vec<CodePoint> buf;
        for (usize i = 0; i < m_size; ++i) {
            if (!pred((*this)[i])) {
                buf.append((*this)[i]);
            }
        }
        const auto length = buf.size();
        auto [size, code_points] = buf.separate();
        return Self(code_points, length, encoding_);
    }

    // /**
    //  * @brief 交换两个字符串
    //  */
    // void swap(const Self& _) noexcept {
    //     // TODO
    // }

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
        auto min_size = std::min(this->length(), other.length());
        for (usize i = 0; i < min_size; ++i) {
            auto cmp = this->operator[](i).__cmp__(other[i]);
            if (cmp != 0) {
                return cmp;
            }
        }
        return static_cast<usize>(this->length() - other.length());
    }

private:
    /**
     * @brief 内部构造函数，用于根据编码创建字符串对象
     * @note 1. 对于SSO，每个位置的码点都会调用默认构造函数初始化
     *       2. 对于heap，每个位置不会调用初始化
     * @param length 字符串的长度
     * @param encoding 字符串的编码
     */
    BasicString(const usize length, Encoding* encoding) :
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
    Pair<usize, usize> get_trim_index(const View& pattern) const {
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
    usize get_ltrim_index(const View& pattern) const {
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
    usize get_rtrim_index(const View& pattern) const {
        const usize l = 0, p_size = pattern.length();
        auto r = length();
        while (l + p_size <= r && slice(r - p_size, r) == pattern) r -= p_size;
        return r;
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
using String = BasicString<Allocator<CodePoint>>;

/**
 * @brief 视图类型别名
 */
using StringView = BasicStringView<String::const_iterator, Allocator<CodePoint>>;

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
struct std::formatter<my::util::BasicString<Alloc>, char> : std::formatter<string_view, char> {
    auto format(const my::util::BasicString<Alloc>& str, auto& ctx) const {
        auto s = str.into_string();
        return std::formatter<std::string_view, char>::format(
            std::string_view(s.data(), s.length()), ctx);
    }
};

#endif // STRING_HPP