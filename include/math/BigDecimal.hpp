/**
 * @brief 高精度十进制数
 * @author Ricky
 * @date 2025/6/5
 * @version 1.0
 */
#ifndef BIG_DECIMAL_HPP
#define BIG_DECIMAL_HPP

#include "BigInteger.hpp"
#include "CString.hpp"
#include "Printer.hpp"
#include "raise_error.hpp"
#include "Pair.hpp"

namespace my::math {

/**
 * @brief 舍入模式枚举
 *
 * 定义了五种舍入模式：
 *   UP: 远离零舍入
 *   DOWN: 向零舍入
 *   CEILING: 向正无穷舍入
 *   FLOOR: 向负无穷舍入
 *   HALF_UP: 四舍五入
 */
enum RoundingMode {
    UP,      // 远离零舍入
    DOWN,    // 向零舍入
    CEILING, // 向正无穷舍入
    FLOOR,   // 向负无穷舍入
    HALF_UP  // 四舍五入
};

class BigDecimal : public Object<BigDecimal> {
public:
    using Self = BigDecimal;

    static const BigDecimal ZERO;
    static const BigDecimal ONE;

    BigDecimal(i32 val = 0) {
        *this = i64(val);
    }

    BigDecimal(i64 val) {
        *this = val;
    }

    // BigDecimal(f64 val) {
    //     // TODO
    // }

    BigDecimal(const CString& val) {
        *this = val;
    }

    BigDecimal(const char* val) {
        *this = val;
    }

    BigDecimal(const BigInteger& unscaled_val, u32 scale) :
            unscaled_value_(unscaled_val), scale_(scale) {
        this->precision_ = calc_precision(unscaled_value_);
    }

    BigDecimal(const Self& other) :
            unscaled_value_(other.unscaled_value_), scale_(other.scale_), precision_(other.precision_) {}

    BigDecimal(Self&& other) noexcept :
            unscaled_value_(std::move(other.unscaled_value_)), scale_(other.scale_), precision_(other.precision_) {}

    Self& operator=(i32 val) {
        *this = i64(val);
        return *this;
    }

    Self& operator=(i64 val) {
        this->unscaled_value_ = val;
        this->precision_ = calc_precision(unscaled_value_);
        return *this;
    }

    // Self& operator=(f64 val) {
    //     i64 integer_part = i64(val);
    //     f64 decimal_part = std::abs(val) - std::abs(integer_part);
    // }

    Self& operator=(const CString& val) {
        if (val.empty()) RuntimeError("Empty string");
        parse_string(val);
        return *this;
    }

    Self& operator=(const char* val) {
        *this = CString(val);
        return *this;
    }

    Self& operator=(const Self& other) {
        if (this == &other) return *this;

        this->unscaled_value_ = other.unscaled_value_;
        this->scale_ = other.scale_;
        this->precision_ = other.precision_;
        return *this;
    }

    Self& operator=(Self&& other) {
        if (this == &other) return *this;

        this->unscaled_value_ = std::move(other.unscaled_value_);
        this->scale_ = other.scale_;
        this->precision_ = other.precision_;
        return *this;
    }

    bool is_zero() const {
        return this->__equals__(ZERO);
    }

    bool is_one() const {
        return this->__equals__(ONE);
    }

    /**
     * @brief 标度控制
     */
    Self scale(u32 new_scale, RoundingMode mode) const {
        if (new_scale == scale_) {
            return *this;
        }

        // TODO
    }

    u32 scale() const noexcept {
        return this->scale_;
    }

    /**
     * @brief 精度管理
     */
    u32 precision() const {
        if (precision_ == 0) {
            precision_ = calc_precision(unscaled_value_);
        }
        return precision_;
    }

    /**
     * @brief 按指定精度舍入
     * @param precision 目标精度
     * @param mode 舍入模式
     * @return 舍入后的新BigDecimal对象
     */
    Self round(u32 precision, RoundingMode mode) const;

    /**
     * @brief 获取绝对值
     * @return 当前对象的绝对值
     */
    Self abs() const {
        return BigDecimal{unscaled_value_.abs(), scale_};
    }

    friend Self operator+(const Self& a, const Self& b) {
        auto [a_aligned, b_aligned] = align_scales(a, b);
        return BigDecimal{
            a_aligned.unscaled_value_ + b_aligned.unscaled_value_,
            a_aligned.scale_};
    }

    Self& operator+=(const Self& other) {
        *this = *this + other;
        return *this;
    }

    Self& operator++() {
        *this = *this + ONE;
        return *this;
    }

    Self operator++(i32) {
        Self ans{*this};
        ++*this;
        return ans;
    }

    Self operator-() const {
        Self ans{*this};
        if (ans != 0) {
            ans.unscaled_value_ = -ans.unscaled_value_;
        }
        return ans;
    }

    friend Self operator-(const Self& a, const Self& b) {
        if (a.is_zero()) return -b;
        if (b.is_zero()) return a;

        auto [a_aligned, b_aligned] = align_scales(a, b);
        return BigDecimal{
            a_aligned.unscaled_value_ - b_aligned.unscaled_value_,
            a_aligned.scale_};
    }

    Self& operator-=(const Self& other) {
        *this = *this - other;
        return *this;
    }

    Self& operator--() {
        *this = *this - ONE;
        return *this;
    }

    Self operator--(i32) {
        Self ans{*this};
        ++*this;
        return ans;
    }

    friend Self operator*(const Self& a, const Self& b) {
        return BigDecimal{
            a.unscaled_value_ * b.unscaled_value_,
            a.scale_ + b.scale_};
    }

    Self& operator*=(const Self& other) {
        *this = *this * other;
        return *this;
    }

    // TODO 调用div函数，默认设置精度
    friend Self operator/(const Self& a, const Self& b);
    Self& operator/=(const Self& other);

    friend bool operator==(const Self& a, const Self& b) {
        return a.__equals__(b);
    }

    friend bool operator!=(const Self& a, const Self& b) {
        return !a.__equals__(b);
    }

    [[nodiscard]] cmp_t __cmp__(const Self& other) const {
        // 比较零值
        if (unscaled_value_.is_zero() && other.unscaled_value_.is_zero()) {
            return 0;
        }

        // 比较符号
        if (unscaled_value_.is_neg() ^ other.unscaled_value_.is_neg()) {
            return unscaled_value_.is_neg() ? 1 : -1;
        }

        // 对齐标度后比较
        auto [a_aligned, b_aligned] = align_scales(*this, other);
        return a_aligned.unscaled_value_.__cmp__(b_aligned.unscaled_value_);
    }

    [[nodiscard]] CString __str__() const {
        if (unscaled_value_.is_zero()) {
            if (scale_ == 0) return "0";
            return "0."_cs + CString::of(scale_, '0');
        }

        // 计算整数和小数部分
        auto divisor = BigInteger(10).pow(scale_);
        auto integer = unscaled_value_ / divisor;
        auto decimal = (unscaled_value_ % divisor).abs(); // 小数部分取绝对值

        auto integer_str = integer.__str__();
        // 处理负零情况
        if (integer.is_zero() && unscaled_value_.is_neg()) {
            integer_str = "-0";
        }

        // 格式化小数部分
        auto decimal_str = decimal.__str__();
        if (scale_ > 0) {
            // 补足前导零
            if (usize len = decimal_str.length(); len < scale_) {
                decimal_str = CString::of(scale_ - len, '0') + decimal_str;
            }
            return integer_str + "."_cs + decimal_str;
        }
        return integer_str;
    }

private:
    /**
     * @brief 解析字符串表示
     * @param cstr 输入字符串
     */
    void parse_string(const CString& cstr) {
        usize curr = 0;
        bool neg = false;

        // 处理符号
        if (cstr[0] == '-') {
            neg = true;
            curr = 1;
        } else if (cstr[0] == '+') {
            curr = 1;
        }

        // 获取整数部分和小数部分
        auto pos = cstr.find('.');
        auto integer_part = (pos == npos ? cstr.slice(curr) : cstr.slice(curr, pos));
        auto decimal_part = (pos == npos ? ""_cs : cstr.slice(pos + 1));

        // 构建未缩放值
        auto unscaled_str = integer_part + decimal_part;
        this->scale_ = static_cast<i32>(decimal_part.length());

        // 移除前导零
        auto first_non_zero = unscaled_str.find_first_not_of('0');
        if (first_non_zero == npos) {
            this->unscaled_value_ = BigInteger::ZERO;
            this->scale_ = 0;
        } else {
            unscaled_str = unscaled_str.slice(first_non_zero);
            this->unscaled_value_ = neg ? -BigInteger(unscaled_str) : BigInteger(unscaled_str);
        }
    }

    /**
     * @brief 计算BigInteger的精度
     * @param val BigInteger值
     * @return 精度（有效数字位数）
     */
    static u32 calc_precision(const BigInteger& val) {
        if (val.is_zero()) {
            return 1;
        }
        // 精度等于数字的位数（忽略前导零）
        return static_cast<u32>(val.abs().__str__().length());
    }

    /**
     * @brief 对齐两个BigDecimal的标度
     * @param a 第一个数
     * @param b 第二个数
     * @return 对齐后的两个数
     */
    static Pair<BigDecimal, BigDecimal> align_scales(const Self& a, const Self& b) {
        if (a.scale_ == b.scale_) {
            return {a, b};
        }

        const Self* scaled = &a;
        const Self* fixed = &b;
        u32 scale_diff = 0;

        if (a.scale_ < b.scale_) {
            scale_diff = b.scale_ - a.scale_;
        } else {
            scale_diff = a.scale_ - b.scale_;
            scaled = &b;
            fixed = &a;
        }

        auto new_unscaled = scaled->unscaled_value_ * BigInteger(10).pow(scale_diff);
        return a.scale_ < b.scale_ ?
                   Pair(Self(new_unscaled, fixed->scale_), b) :
                   Pair(a, Self(new_unscaled, fixed->scale_));
    }

    /**
     * @brief 执行除法运算
     * @param dividend 被除数
     * @param divisor 除数
     * @param precision 精度
     * @param mode 舍入模式
     * @return 除法结果
     */
    static Self divide(const Self& dividend, const Self& divisor,
                       u32 precision, RoundingMode mode);

    /**
     * @brief 应用舍入模式
     * @param value 要舍入的值
     * @param mode 舍入模式
     * @return 舍入后的值
     */
    static BigInteger apply_rounding(const BigInteger& value,
                                     RoundingMode mode,
                                     const BigInteger& remainder,
                                     const BigInteger& divisor);

    /**
     * @brief 检查是否需要进位
     * @param remainder 余数
     * @param divisor 除数
     * @param mode 舍入模式
     * @return 是否需要进位
     */
    static bool need_increment(const BigInteger& remainder,
                               const BigInteger& divisor,
                               RoundingMode mode);

private:
    BigInteger unscaled_value_; // 未缩放整数值（含符号）
    u32 scale_ = 0;             // 小数点右移位数（非负数）
    mutable u32 precision_ = 0; // 精度（有效数字位数）
};

inline const BigDecimal BigDecimal::ZERO = 0;
inline const BigDecimal BigDecimal::ONE = 1;

} // namespace my::math

#endif // BIG_DECIMAL_HPP