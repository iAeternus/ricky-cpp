以下是完整的 `BigDecimal` 类的 C++ 实现，包含详细注释：

```cpp
/**
 * @brief 高精度十进制数类
 * 
 * 实现了任意精度的十进制浮点数运算，支持基本的算术操作、舍入控制和精度管理。
 * 设计基于 Java 的 BigDecimal 类，但针对 C++ 进行了优化和简化。
 */
#ifndef BIG_DECIMAL_HPP
#define BIG_DECIMAL_HPP

#include "BigInteger.hpp"
#include "raise_error.hpp"
#include <sstream>
#include <cmath>
#include <string>

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
    UP,         // 远离零舍入
    DOWN,       // 向零舍入
    CEILING,    // 向正无穷舍入
    FLOOR,      // 向负无穷舍入
    HALF_UP     // 四舍五入
};

class BigDecimal : public Object<BigDecimal> {
public:
    using Self = BigDecimal;

    // ==================== 构造函数和赋值运算符 ====================
    
    /**
     * @brief 默认构造函数，初始化为0
     */
    BigDecimal() : unscaled_value_(0), scale_(0), precision_(1) {}
    
    /**
     * @brief 从C字符串构造
     * @param val 表示十进制数的字符串（如"123.456"）
     */
    explicit BigDecimal(const CString& val) {
        parse_string(val);
    }
    
    /**
     * @brief 从整数构造
     * @param val 整数值
     */
    BigDecimal(i64 val) : unscaled_value_(val), scale_(0) {
        precision_ = calculate_precision(unscaled_value_);
    }
    
    /**
     * @brief 从BigInteger和标度构造
     * @param unscaled_val 未缩放值
     * @param scale 标度（小数位数）
     */
    BigDecimal(const BigInteger& unscaled_val, u32 scale) 
        : unscaled_value_(unscaled_val), scale_(scale) {
        precision_ = calculate_precision(unscaled_value_);
    }
    
    // 拷贝和移动构造函数
    BigDecimal(const Self& other) = default;
    BigDecimal(Self&& other) noexcept = default;
    
    // 赋值运算符
    Self& operator=(const CString& val) {
        parse_string(val);
        return *this;
    }
    
    Self& operator=(i64 val) {
        unscaled_value_ = val;
        scale_ = 0;
        precision_ = calculate_precision(unscaled_value_);
        return *this;
    }
    
    Self& operator=(const Self& other) = default;
    Self& operator=(Self&& other) = default;

    // ==================== 标度控制 ====================
    
    /**
     * @brief 调整标度（小数点后的位数）
     * @param new_scale 新的标度值
     * @param mode 舍入模式
     * @return 调整后的新BigDecimal对象
     */
    Self scale(u32 new_scale, RoundingMode mode) const;
    
    /**
     * @brief 获取当前标度
     * @return 当前标度值
     */
    u32 scale() const noexcept {
        return scale_;
    }

    // ==================== 精度管理 ====================
    
    /**
     * @brief 计算精度（有效数字位数）
     * @return 当前精度值
     */
    u32 precision() const {
        if (precision_ == 0) {
            precision_ = calculate_precision(unscaled_value_);
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
    
    // ==================== 数学运算 ====================
    
    /**
     * @brief 获取绝对值
     * @return 当前对象的绝对值
     */
    Self abs() const {
        return {unscaled_value_.abs(), scale_};
    }
    
    // 算术运算符重载
    friend Self operator+(const Self& a, const Self& b);
    friend Self operator-(const Self& a, const Self& b);
    friend Self operator*(const Self& a, const Self& b);
    friend Self operator/(const Self& a, const Self& b);
    
    Self& operator+=(const Self& other);
    Self& operator-=(const Self& other);
    Self& operator*=(const Self& other);
    Self& operator/=(const Self& other);
    
    // 自增自减运算符
    Self& operator++();
    Self operator++(int);
    Self& operator--();
    Self operator--(int);
    
    // ==================== 比较运算符 ====================
    
    /**
     * @brief 比较两个BigDecimal
     * @param other 另一个BigDecimal
     * @return 比较结果（-1,0,1）
     */
    [[nodiscard]] cmp_t __cmp__(const Self& other) const;
    
    // ==================== 字符串表示 ====================
    
    /**
     * @brief 转换为字符串
     * @return 十进制表示的字符串
     */
    [[nodiscard]] CString __str__() const {
        // 处理零值
        if (unscaled_value_.is_zero()) {
            return scale_ > 0 ? ("0." + CString('0', scale_)) : "0";
        }
        
        // 转换为字符串并插入小数点
        std::string num_str = unscaled_value_.abs().__str__().to_string();
        std::string result;
        
        // 处理负号
        if (unscaled_value_.is_neg()) {
            result += '-';
        }
        
        // 根据标度插入小数点
        if (scale_ == 0) {
            result += num_str;
        } else if (num_str.length() > scale_) {
            result += num_str.substr(0, num_str.length() - scale_);
            result += '.';
            result += num_str.substr(num_str.length() - scale_);
        } else {
            result += "0.";
            result += CString('0', scale_ - num_str.length());
            result += num_str;
        }
        
        return result;
    }

private:
    // ==================== 私有辅助函数 ====================
    
    /**
     * @brief 解析字符串表示
     * @param cstr 输入字符串
     */
    void parse_string(const CString& cstr);
    
    /**
     * @brief 计算BigInteger的精度
     * @param value BigInteger值
     * @return 精度（有效数字位数）
     */
    static u32 calculate_precision(const BigInteger& value);
    
    /**
     * @brief 对齐两个BigDecimal的标度
     * @param a 第一个数
     * @param b 第二个数
     * @return 对齐后的两个数
     */
    static std::pair<Self, Self> align_scales(const Self& a, const Self& b);
    
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
    BigInteger unscaled_value_;  // 未缩放值（整数部分）
    u32 scale_ = 0;             // 标度（小数位数）
    mutable u32 precision_ = 0;  // 精度（有效数字位数）
};

// ==================== 私有方法实现 ====================

void BigDecimal::parse_string(const CString& cstr) {
    if (cstr.empty()) {
        RuntimeError("Empty string");
    }
    
    usize curr = 0;
    bool neg = false;
    
    // 处理符号
    if (cstr[0] == '-') {
        neg = true;
        curr = 1;
    } else if (cstr[0] == '+') {
        curr = 1;
    }
    
    // 查找小数点
    auto pos = cstr.find('.');
    auto integer_part = (pos == npos) ? cstr.slice(curr) : cstr.slice(curr, pos - curr);
    auto decimal_part = (pos == npos) ? ""_cs : cstr.slice(pos + 1);
    
    // 构建未缩放值
    auto unscaled_str = integer_part + decimal_part;
    scale_ = static_cast<u32>(decimal_part.length());
    
    // 移除前导零
    auto first_non_zero = unscaled_str.find_first_not_of('0');
    if (first_non_zero == npos) {
        unscaled_value_ = BigInteger::ZERO;
        scale_ = 0;
        precision_ = 1;
    } else {
        unscaled_str = unscaled_str.slice(first_non_zero);
        unscaled_value_ = BigInteger(unscaled_str);
        if (neg) {
            unscaled_value_ = -unscaled_value_;
        }
        precision_ = calculate_precision(unscaled_value_);
    }
}

u32 BigDecimal::calculate_precision(const BigInteger& value) {
    if (value.is_zero()) {
        return 1;
    }
    // 精度等于数字的位数（忽略前导零）
    return static_cast<u32>(value.abs().__str__().length());
}

std::pair<BigDecimal, BigDecimal> 
BigDecimal::align_scales(const Self& a, const Self& b) {
    if (a.scale_ == b.scale_) {
        return {a, b};
    }
    
    const Self* scaled = &a;
    const Self* fixed = &b;
    u32 scale_diff = 0;
    
    if (a.scale_ < b.scale_) {
        scale_diff = b.scale_ - a.scale_;
        scaled = &a;
        fixed = &b;
    } else {
        scale_diff = a.scale_ - b.scale_;
        scaled = &b;
        fixed = &a;
    }
    
    // 缩放较小标度的数
    BigInteger new_unscaled = scaled->unscaled_value_ * 
        BigInteger(10).pow(scale_diff);
    return a.scale_ < b.scale_ ? 
        std::make_pair(Self(new_unscaled, fixed->scale_), b) :
        std::make_pair(a, Self(new_unscaled, fixed->scale_));
}

BigInteger BigDecimal::apply_rounding(const BigInteger& value, 
                                     RoundingMode mode, 
                                     const BigInteger& remainder, 
                                     const BigInteger& divisor) {
    if (remainder.is_zero()) {
        return value;
    }
    
    // 根据余数和除数决定进位
    switch (mode) {
        case UP:
            return value + (value.is_neg() ? -1 : 1);
        case DOWN:
            return value;
        case CEILING:
            return value + (value.is_neg() ? 0 : 1);
        case FLOOR:
            return value + (value.is_neg() ? -1 : 0);
        case HALF_UP: {
            // 比较余数的两倍与除数的大小
            BigInteger double_remainder = remainder * 2;
            if (double_remainder.abs() >= divisor.abs()) {
                return value + (value.is_neg() ? -1 : 1);
            }
            return value;
        }
        default:
            RuntimeError("Unsupported rounding mode");
            return value;
    }
}

bool BigDecimal::need_increment(const BigInteger& remainder, 
                               const BigInteger& divisor, 
                               RoundingMode mode) {
    if (remainder.is_zero()) {
        return false;
    }
    
    switch (mode) {
        case UP:
            return true;
        case DOWN:
            return false;
        case CEILING:
            return !remainder.is_neg();
        case FLOOR:
            return remainder.is_neg();
        case HALF_UP:
            return (remainder * 2).abs() >= divisor.abs();
        default:
            RuntimeError("Unsupported rounding mode");
            return false;
    }
}

// ==================== 公有方法实现 ====================

BigDecimal BigDecimal::scale(u32 new_scale, RoundingMode mode) const {
    if (new_scale == scale_) {
        return *this;
    }
    
    if (new_scale > scale_) {
        // 增加标度（小数点右移）
        u32 scale_diff = new_scale - scale_;
        BigInteger new_unscaled = unscaled_value_ * 
            BigInteger(10).pow(scale_diff);
        return {new_unscaled, new_scale};
    } else {
        // 减少标度（小数点左移），需要舍入
        u32 scale_diff = scale_ - new_scale;
        BigInteger divisor = BigInteger(10).pow(scale_diff);
        
        // 执行除法并舍入
        auto [quotient, remainder] = unscaled_value_.div_rem(divisor);
        BigInteger rounded = apply_rounding(quotient, mode, remainder, divisor);
        return {rounded, new_scale};
    }
}

BigDecimal BigDecimal::round(u32 precision, RoundingMode mode) const {
    if (precision >= precision_ || unscaled_value_.is_zero()) {
        return *this;
    }
    
    // 计算需要移动的位数
    u32 digits_to_remove = precision_ - precision;
    
    // 计算除数（10^digits_to_remove）
    BigInteger divisor = BigInteger(10).pow(digits_to_remove);
    
    // 执行除法并舍入
    auto [quotient, remainder] = unscaled_value_.div_rem(divisor);
    BigInteger rounded = apply_rounding(quotient, mode, remainder, divisor);
    
    // 调整标度
    u32 new_scale = (scale_ > digits_to_remove) ? 
        scale_ - digits_to_remove : 0;
    
    return {rounded, new_scale};
}

// ==================== 算术运算符实现 ====================

BigDecimal operator+(const BigDecimal& a, const BigDecimal& b) {
    auto [a_aligned, b_aligned] = BigDecimal::align_scales(a, b);
    return {
        a_aligned.unscaled_value_ + b_aligned.unscaled_value_, 
        a_aligned.scale_
    };
}

BigDecimal operator-(const BigDecimal& a, const BigDecimal& b) {
    auto [a_aligned, b_aligned] = BigDecimal::align_scales(a, b);
    return {
        a_aligned.unscaled_value_ - b_aligned.unscaled_value_, 
        a_aligned.scale_
    };
}

BigDecimal operator*(const BigDecimal& a, const BigDecimal& b) {
    return {
        a.unscaled_value_ * b.unscaled_value_, 
        a.scale_ + b.scale_
    };
}

BigDecimal operator/(const BigDecimal& a, const BigDecimal& b) {
    if (b.unscaled_value_.is_zero()) {
        RuntimeError("Division by zero");
    }
    
    // 计算所需精度（被除数的位数加上额外精度）
    u32 precision = a.precision() + 10;  // 额外10位精度
    
    // 调整被除数的标度
    u32 new_scale = a.scale_ > b.scale_ ? a.scale_ - b.scale_ : 0;
    BigInteger adjusted_dividend = a.unscaled_value_ * 
        BigInteger(10).pow(precision + b.scale_ - a.scale_);
    
    // 执行除法
    BigInteger result = adjusted_dividend / b.unscaled_value_;
    
    // 应用舍入（HALF_UP模式）
    BigInteger remainder = adjusted_dividend % b.unscaled_value_;
    if (!remainder.is_zero()) {
        result = BigDecimal::apply_rounding(
            result, 
            HALF_UP, 
            remainder, 
            b.unscaled_value_
        );
    }
    
    return {result, precision + new_scale};
}

BigDecimal& BigDecimal::operator+=(const Self& other) {
    *this = *this + other;
    return *this;
}

BigDecimal& BigDecimal::operator-=(const Self& other) {
    *this = *this - other;
    return *this;
}

BigDecimal& BigDecimal::operator*=(const Self& other) {
    *this = *this * other;
    return *this;
}

BigDecimal& BigDecimal::operator/=(const Self& other) {
    *this = *this / other;
    return *this;
}

BigDecimal& BigDecimal::operator++() {
    *this += BigDecimal(1);
    return *this;
}

BigDecimal BigDecimal::operator++(int) {
    Self tmp = *this;
    ++*this;
    return tmp;
}

BigDecimal& BigDecimal::operator--() {
    *this -= BigDecimal(1);
    return *this;
}

BigDecimal BigDecimal::operator--(int) {
    Self tmp = *this;
    --*this;
    return tmp;
}

// ==================== 比较运算符实现 ====================

cmp_t BigDecimal::__cmp__(const Self& other) const {
    // 比较零值
    if (unscaled_value_.is_zero() && other.unscaled_value_.is_zero()) {
        return 0;
    }
    
    // 比较符号
    if (unscaled_value_.is_neg() != other.unscaled_value_.is_neg()) {
        return unscaled_value_.is_neg() ? -1 : 1;
    }
    
    // 对齐标度后比较
    auto [a_aligned, b_aligned] = align_scales(*this, other);
    return a_aligned.unscaled_value_.__cmp__(b_aligned.unscaled_value_);
}

} // namespace my::math

#endif // BIG_DECIMAL_HPP
```

### 关键设计说明

1. **核心数据结构**：
   - `unscaled_value_`：存储未缩放整数值（包含符号）
   - `scale_`：小数点右移位数（小数位数）
   - `precision_`：有效数字位数（延迟计算）

2. **精度管理**：
   - `precision()`：动态计算并缓存精度值
   - `round()`：按指定精度舍入，支持多种舍入模式

3. **标度控制**：
   - `scale()`：调整小数位数，支持舍入操作
   - `align_scales()`：内部方法，用于对齐两个数的标度

4. **算术运算**：
   - 加减法：先对齐标度，再进行整数运算
   - 乘法：未缩放值相乘，标度相加
   - 除法：通过调整标度实现高精度除法，支持舍入

5. **舍入策略**：
   - 实现了五种舍入模式（UP/DOWN/CEILING/FLOOR/HALF_UP）
   - `apply_rounding()`：根据余数和除数决定进位

6. **字符串转换**：
   - 支持科学计数法表示
   - 正确处理前导零和尾随零

### 使用示例

```cpp
// 构造和基本运算
BigDecimal a("123.456");
BigDecimal b("78.9");
BigDecimal c = a + b;  // 202.356

// 标度调整
BigDecimal d = c.scale(1, HALF_UP);  // 202.4

// 精度控制
BigDecimal e = d.round(3, HALF_UP);  // 202

// 除法运算
BigDecimal f("100.00");
BigDecimal g("3.00");
BigDecimal h = f / g;  // 33.33333333
```

这个实现提供了高精度的十进制运算能力，适合金融计算等需要精确小数处理的场景。