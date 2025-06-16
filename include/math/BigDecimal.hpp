/**
 * @brief 高精度十进制数
 * @details 支持任意精度的十进制数计算，包括加减乘除和比较操作
 * @author Ricky
 * @date 2025/6/5
 * @version 1.1
 */
#ifndef BIG_DECIMAL_HPP
#define BIG_DECIMAL_HPP

#include "BigInteger.hpp"
#include "CString.hpp"
#include "raise_error.hpp"
#include "Pair.hpp"

namespace my::math {

/**
 * @brief 舍入模式枚举
 * @details 定义了不同的数值舍入策略
 */
enum RoundingMode {
    UP,      // 远离零舍入（向绝对值更大的方向舍入）
    DOWN,    // 向零舍入（向绝对值更小的方向舍入）
    CEILING, // 向正无穷舍入（向更大的数字方向舍入）
    FLOOR,   // 向负无穷舍入（向更小的数字方向舍入）
    HALF_UP  // 四舍五入（>=5向上舍入，<5向下舍入）
};

class BigDecimal : public Object<BigDecimal> {
public:
    using Self = BigDecimal;

    static const BigDecimal ZERO;
    static const BigDecimal ONE;
    static const BigDecimal TEN;

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

    bool is_pos() const {
        return unscaled_value_.is_pos();
    }

    bool is_neg() const {
        return unscaled_value_.is_neg();
    }

    bool is_zero() const {
        return this->__equals__(ZERO);
    }

    bool is_one() const {
        return this->__equals__(ONE);
    }

    /**
     * @brief 调整标度（小数点后的位数）
     * @param new_scale 新的标度值
     * @param mode 舍入模式
     * @return 调整后的新BigDecimal对象
     */
    Self scale(u32 new_scale, RoundingMode mode = HALF_UP) const {
        if (new_scale == scale_) {
            return *this;
        }

        if (new_scale > scale_) {
            // 增加标度（小数点右移）
            auto scale_diff = new_scale - scale_;
            auto new_unscaled = unscaled_value_ * BigInteger(10).pow(scale_diff);
            return Self{new_unscaled, new_scale};
        } else {
            // 减少标度（小数点左移），需要舍入
            auto scale_diff = scale_ - new_scale;
            auto divisor = BigInteger(10).pow(scale_diff);
            // 执行除法并舍入
            auto [quotient, remainder] = unscaled_value_.div_rem(divisor);
            auto rounded = apply_rounding(quotient, mode, remainder, divisor);
            return Self{rounded, new_scale};
        }
    }

    u32 scale() const noexcept {
        return this->scale_;
    }

    /**
     * @brief 精度管理
     */
    u32 precision() const {
        if (precision_ == 0) {
            Self tmp = this->strip_trailing_zeros(); // 去除尾部零
            precision_ = calc_precision(tmp.unscaled_value_);
        }
        return precision_;
    }

    /**
     * @brief 按有效数字位数（精度）进行舍入
     * @param precision 目标精度
     * @param mode 舍入模式
     * @return 舍入后的新BigDecimal对象
     */
    Self round(u32 precision, RoundingMode mode = HALF_UP) const {
        if (precision == 0 || is_zero()) {
            return ZERO;
        }

        u32 current_prec = this->precision();
        if (precision >= current_prec) {
            return *this;
        }

        // 计算需要移除的位数
        u32 roundingPos = current_prec - precision;
        BigInteger divisor = BigInteger(10).pow(roundingPos);

        // 取绝对值处理
        BigInteger abs_unscaled = unscaled_value_.abs();
        auto [quotient, remainder] = abs_unscaled.div_rem(divisor);

        // 应用舍入
        BigInteger rounded_quotient = apply_rounding(quotient, mode, remainder, divisor);

        // 恢复符号并构造新值
        if (unscaled_value_.is_neg()) {
            rounded_quotient = -rounded_quotient;
        }
        BigInteger new_unscaled = rounded_quotient * divisor;

        return Self(new_unscaled, scale_);
    }

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

    /**
     * @brief 除法运算
     * @param other 除数
     * @param scale 结果小数位数（即结果的scale）
     * @param mode 舍入模式
     * @return 除法运算结果
     */
    Self divide(const Self& other, u32 scale, RoundingMode mode = HALF_UP) const {
        if (other.is_zero()) {
            RuntimeError("Division by zero");
        }

        // 如果被除数为0，直接返回0
        if (this->is_zero()) {
            return ZERO;
        }

        // 计算指数 = 结果scale + 除数scale - 被除数scale
        auto exponent = static_cast<i32>(scale) + static_cast<i32>(other.scale_) - static_cast<i32>(this->scale_);

        auto dividend = this->unscaled_value_;
        auto divisor = other.unscaled_value_;

        // 处理指数移位
        if (exponent > 0) {
            dividend = dividend.left_shift(exponent); // 被除数左移
        } else if (exponent < 0) {
            divisor = divisor.left_shift(-exponent); // 除数左移
        }

        // 执行整数除法
        auto [quotient, remainder] = dividend.div_rem(divisor);

        // 处理余数符号：确保余数符号与除数一致
        if (!remainder.is_zero()) {
            if ((dividend.is_neg() && !divisor.is_neg()) || (!dividend.is_neg() && divisor.is_neg())) {
                remainder = -remainder;
            }
        }

        // 应用舍入模式
        auto rounded = apply_rounding(quotient, mode, remainder, divisor);

        // 创建结果并设置正确的标度
        return Self(rounded, scale);
    }

    /**
     * @brief 除法运算符
     * @note 以a和b标度的较大者为结果的标度
     * @param other 除数
     * @return 除法结果
     */
    friend Self operator/(const Self& a, const Self& b) {
        return a.divide(b, std::max(a.scale_, b.scale_), HALF_UP);
    }

    Self& operator/=(const Self& other) {
        *this = *this / other;
        return *this;
    }

    Self pow(u32 exponent) const {
        Self result(1);
        for (u32 i = 0; i < exponent; ++i) {
            result *= *this;
        }
        return result;
    }

    /**
     * @brief 计算平方根
     * @param precision 结果的精度（小数位数）
     * @return 平方根结果
     */
    Self sqrt(u32 precision = 10) const {
        if (is_neg())
            RuntimeError("Cannot calculate square root of negative number");
        if (is_zero())
            return ZERO.scale(precision, HALF_UP);

        // 初始估计值（保持足够精度）
        Self x = *this * Self(10).pow(precision * 2);
        Self last;
        Self two(2);
        u32 calc_precision = precision * 2; // 计算时使用双倍精度

        do {
            last = x;
            x = (x + *this / x) / two;
            x = x.scale(calc_precision, HALF_UP);
        } while (last != x);

        return x.scale(precision, HALF_UP);
    }

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

        // 处理负号
        bool is_negative = unscaled_value_.is_neg();
        BigInteger abs_value = unscaled_value_.abs();

        // 计算整数和小数部分
        auto divisor = BigInteger(10).pow(scale_);
        auto [integer, decimal] = abs_value.div_rem(divisor);

        CString integer_str = integer.__str__();
        // 添加负号
        if (is_negative) {
            integer_str = "-"_cs + integer_str;
        }

        // 格式化小数部分
        if (scale_ > 0) {
            CString decimal_str = decimal.__str__();
            // 补足前导零
            if (usize len = decimal_str.length(); len < scale_) {
                decimal_str = CString::of(scale_ - len, '0') + decimal_str;
            }
            return integer_str + "."_cs + decimal_str;
        }
        return integer_str;
    }

    /**
     * @brief 小数点左移
     * @param n 左移位数
     * @return 移动后的新BigDecimal对象
     */
    Self move_point_left(u32 n) const {
        // 直接增加标度值
        return Self{unscaled_value_, scale_ + n};
    }

    /**
     * @brief 小数点右移
     * @param n 右移位数
     * @return 移动后的新BigDecimal对象
     */
    Self move_point_right(u32 n) const {
        if (n > scale_) {
            // 如果右移超过当前标度，需要在末尾补零
            auto extra_zeros = n - scale_;
            return Self{unscaled_value_.left_shift(extra_zeros), 0};
        }
        return Self{unscaled_value_, scale_ - n};
    }

    /**
     * @brief 去除尾部的零
     * @return 去除尾部零后的新BigDecimal对象
     */
    Self strip_trailing_zeros() const {
        if (unscaled_value_.is_zero()) {
            return ZERO;
        }

        auto str = unscaled_value_.__str__();
        u32 trailing_zeros = 0;
        for (isize i = static_cast<isize>(str.length()) - 1; i >= 0; --i) {
            if (str.data()[i] == '0') {
                ++trailing_zeros;
            } else {
                break;
            }
        }

        trailing_zeros = trailing_zeros > scale_ ? scale_ : trailing_zeros;
        if (trailing_zeros == 0) {
            return *this;
        }

        return Self{unscaled_value_ / BigInteger(10).pow(trailing_zeros), scale_ - trailing_zeros};
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

    static u32 calc_scale_diff(u32 scale1, u32 scale2) {
        return scale1 < scale2 ? scale2 - scale1 : scale1 - scale2;
    }

    /**
     * @brief 计算BigInteger的精度，不考虑前导零
     * @param val BigInteger值
     * @return 精度（有效数字位数）
     */
    static u32 calc_precision(const BigInteger& val) {
        if (val.is_zero()) {
            return 1;
        }
        // 精度等于数字的位数
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

        u32 max_scale = std::max(a.scale_, b.scale_);
        return Pair{a.scale_ == max_scale ? a : a.scale(max_scale, DOWN),
                    b.scale_ == max_scale ? b : b.scale(max_scale, DOWN)};
    }

    /**
     * @brief 应用舍入模式
     * @details 根据指定的舍入模式对商进行舍入
     * @param quotient 商
     * @param mode 舍入模式
     * @param remainder 余数
     * @param divisor 除数
     * @return 舍入后的商
     */
    static BigInteger apply_rounding(const BigInteger& quotient,
                                     RoundingMode mode,
                                     const BigInteger& remainder,
                                     const BigInteger& divisor) {
        if (remainder.is_zero()) {
            return quotient;
        }

        // 计算余数的两倍，用于HALF_UP模式的比较
        BigInteger double_remainder = remainder.abs() * 2;
        bool should_round_up = false;

        switch (mode) {
        case UP:
            should_round_up = true;
            break;
        case DOWN:
            should_round_up = false;
            break;
        case CEILING:
            should_round_up = !quotient.is_neg();
            break;
        case FLOOR:
            should_round_up = quotient.is_neg();
            break;
        case HALF_UP:
            should_round_up = double_remainder >= divisor.abs();
            break;
        }

        if (should_round_up) {
            return quotient + (quotient.is_neg() ? -1 : 1);
        }
        return quotient;
    }

private:
    BigInteger unscaled_value_; // 未缩放整数值（含符号）
    u32 scale_ = 0;             // 小数点右移位数（非负数）
    mutable u32 precision_ = 0; // 精度（有效数字位数）
};

inline const BigDecimal BigDecimal::ZERO{0};
inline const BigDecimal BigDecimal::ONE{1};
inline const BigDecimal BigDecimal::TEN{10};

} // namespace my::math

#endif // BIG_DECIMAL_HPP
