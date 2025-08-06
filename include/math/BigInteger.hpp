/**
 * @brief 高精度整数类
 * @details 支持任意精度的整数计算，包括加减乘除、位运算和比较操作
 * @author Ricky
 * @date 2025/1/12
 * @version 1.1
 */
#ifndef BIG_INTEGER_HPP
#define BIG_INTEGER_HPP

#include "math_utils.hpp"
#include "Vec.hpp"

#include <cstring>
#include <format>

namespace my::math {

class BigInteger : public Object<BigInteger> {
public:
    using Self = BigInteger;
    static const Self ZERO;
    static const Self ONE;
    static const Self TWO;
    static const Self TEN;

    /**
     * @brief 默认构造函数，创建值为0的大整数
     */
    BigInteger() :
            sign_(true), length_(1), num_(1, 0) {}

    /**
     * @brief 从32位整数构造
     */
    BigInteger(i32 n) {
        *this = i64(n);
    }

    /**
     * @brief 从64位整数构造
     */
    BigInteger(i64 n) {
        *this = n;
    }

    /**
     * @brief 从字符串构造
     */
    BigInteger(const char* str) {
        *this = str;
    }

    /**
     * @brief 从CString构造
     */
    BigInteger(const CString& str) {
        *this = str;
    }

    /**
     * @brief 拷贝构造函数
     */
    BigInteger(const Self& other) = default;

    /**
     * @brief 移动构造函数
     */
    BigInteger(Self&& other) noexcept = default;

    /**
     * @brief 从32位整数赋值
     */
    Self& operator=(i32 n) {
        *this = i64(n);
        return *this;
    }

    /**
     * @brief 从64位整数赋值
     */
    Self& operator=(i64 n) {
        num_.clear();
        if (n == 0) {
            *this = ZERO;
            return *this;
        }
        if (n > 0) {
            sign_ = true;
        } else if (n == I64_MIN) {
            *this = "-9223372036854775808";
            return *this;
        } else {
            sign_ = false;
            n = -n;
        }
        while (n > 0) {
            num_.append(n % BASE);
            n /= BASE;
        }
        calc_len();
        return *this;
    }

    /**
     * @brief 从字符串赋值
     */
    Self& operator=(const char* str) {
        if (!str || !*str) throw runtime_exception("invalid string");

        auto len = std::strlen(str);
        i32 tmp = 0, ten = 1, stop = 0;
        num_.clear();
        sign_ = (str[0] != '-');
        if (!sign_) stop = 1;

        for (isize i = len; i > stop; --i) {
            if (!std::isdigit(str[i - 1])) throw runtime_exception("invalid character in string");
            tmp += c2i(str[i - 1]) * ten;
            ten *= 10;
            if ((len - i) % WIDTH + 1 == WIDTH) {
                num_.append(tmp);
                tmp = 0;
                ten = 1;
            }
        }
        if ((len - stop) % WIDTH != 0) {
            num_.append(tmp);
        }
        calc_len();
        return *this;
    }

    Self& operator=(const CString& str) {
        *this = str.data();
        return *this;
    }

    Self& operator=(const Self& other) = default;
    Self& operator=(Self&& other) noexcept = default;

    usize size() const {
        return length_;
    }

    /**
    * @brief 判断是否为奇数
    * @return true=是 false=否
    */
    bool is_odd() const {
        return num_.front() & 1;
    }

    /**
    * @brief 判断是否为偶数
    * @return true=是 false=否
    */
    bool is_even() const {
        return !is_odd();
    }

    bool is_pos() const {
        return sign_;
    }

    bool is_neg() const {
        return !sign_;
    }

    bool is_zero() const {
        return this->__equals__(ZERO);
    }

    bool is_one() const {
        return this->__equals__(ONE);
    }

    /**
    * @brief 左移n个十进制位，低位补0
    */
    Self left_shift(usize n) const {
        auto bit_shift = n % WIDTH;
        Self ans;
        ans.length_ = n + 1;
        n /= WIDTH;
        while (ans.num_.size() <= n) {
            ans.num_.append(0);
        }
        ans.num_[n] = 1;
        while (bit_shift--) {
            ans.num_[n] *= 10;
        }
        return ans * (*this);
    }

    /**
    * @brief 右移n个十进制位，丢弃低位，TODO 运算会导致性能问题，有待优化
    */
    Self right_shift(usize n) const {
        if (n >= length_) {
            return {0};
        }
        return *this / TEN.pow(n);
    }

    Self abs() const {
        Self ans{*this};
        ans.sign_ = true;
        return ans;
    }

    const Self& operator+() const {
        return *this;
    }

    friend Self operator+(const Self& a, const Self& b) {
        if (!b.sign_) {
            return a - (-b);
        }
        if (!a.sign_) {
            return b - (-a);
        }
        Self ans;
        i32 carry = 0, aa, bb;
        auto a_size = a.num_.size(), b_size = b.num_.size();
        auto max_size = math::max_(a_size, b_size);
        ans.num_.clear();
        for (usize i = 0; i < max_size; ++i) {
            aa = a_size <= i ? 0 : a.num_[i];
            bb = b_size <= i ? 0 : b.num_[i];
            ans.num_.append((aa + bb + carry) % BASE);
            carry = (aa + bb + carry) / BASE;
        }
        if (carry > 0) {
            ans.num_.append(carry);
        }
        ans.calc_len();
        return ans;
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
        if (!ans.is_zero()) {
            ans.sign_ = !ans.sign_;
        }
        return ans;
    }

    friend Self operator-(const Self& a, const Self& b) {
        if (!b.sign_) {
            return a + (-b);
        }
        if (!a.sign_) {
            return -((-a) + b);
        }
        if (a < b) {
            return -(b - a);
        }
        Self ans;
        i32 carry = 0, aa, bb;
        usize a_size = a.num_.size(), b_size = b.num_.size();
        usize max_size = math::max_(a_size, b_size);
        ans.num_.clear();
        for (usize i = 0; i < max_size; ++i) {
            aa = a.num_[i];
            bb = b_size <= i ? 0 : b.num_[i];
            ans.num_.append((aa - bb - carry + BASE) % BASE);
            carry = aa < bb + carry ? 1 : 0;
        }
        ans.calc_len();
        return ans;
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
        --*this;
        return ans;
    }

    friend Self operator*(const Self& a, const Self& b) {
        auto a_size = a.num_.size(), b_size = b.num_.size();
        util::Vec<i64> res;
        for (usize i = 0; i < a_size; ++i) {
            for (usize j = 0; j < b_size; ++j) {
                i64 tmp = i64(a.num_[i]) * i64(b.num_[j]);
                i + j < res.size() ? res[i + j] += tmp : res.append(tmp);
            }
        }
        while (res.back() == 0 && res.size() != 1) {
            res.pop();
        }
        Self ans;
        ans.sign_ = a.sign_ == b.sign_ || (res.size() == 1 && res[0] == 0);
        ans.num_.clear();
        auto resSize = res.size();
        i64 carry = 0, tmp;
        for (usize i = 0; i < resSize; ++i) {
            tmp = res[i];
            ans.num_.append((tmp + carry) % BASE);
            carry = (tmp + carry) / BASE;
        }
        if (carry > 0) {
            ans.num_.append(carry);
        }
        ans.calc_len();
        return ans;
    }

    Self& operator*=(const Self& other) {
        *this = *this * other;
        return *this;
    }

    // friend Self operator/(const Self& a, const Self& b) {
    //     if (b.is_zero()) {
    //         ValueError("Divide by zero");
    //     }
    //     Self aa{a.abs()}, bb{b.abs()};
    //     if (aa < bb) {
    //         return ZERO;
    //     }
    //     CString str(aa.size() + 1);
    //     std::memset(str.data(), 0, str.size());
    //     Self tmp;
    //     auto lenDiff = aa.length_ - bb.length_;
    //     for (usize i = 0; i <= lenDiff; ++i) {
    //         tmp = bb.left_shift(lenDiff - i);
    //         while (aa >= tmp) {
    //             ++str[i];
    //             aa -= tmp;
    //         }
    //         str[i] = i2c(str[i]);
    //     }
    //     Self ans{str};
    //     ans.sign_ = a.sign_ == b.sign_ || ans.is_zero();
    //     return ans;
    // }

    friend Self operator/(const Self& a, const Self& b) {
        if (b.is_zero()) {
            throw arithmetic_exception("/ by zero");
        }
        Self aa = a.abs();
        Self bb = b.abs();

        if (aa < bb) {
            return ZERO;
        }

        // 使用更精确的除法算法
        Self quotient;
        Self current;
        auto a_digits = aa.__str__();

        for (char c : a_digits) {
            current = current * TEN + Self(c - '0');
            if (current < bb) {
                quotient = quotient * TEN;
                continue;
            }

            int digit = 0;
            while (current >= bb) {
                current -= bb;
                digit++;
            }
            quotient = quotient * TEN + Self(digit);
        }

        quotient.sign_ = a.sign_ == b.sign_;
        return quotient;
    }

    Self& operator/=(const Self& other) {
        *this = *this / other;
        return *this;
    }

    friend Self operator%(const Self& a, const Self& b) {
        return a - a / b * b;
    }

    Self& operator%=(const Self& other) {
        *this = *this % other;
        return *this;
    }

    /**
    * @brief 幂运算，base^exp
    */
    friend Self operator^(Self base, Self exp) {
        Self ans{ONE};
        while (!exp.is_zero()) {
            if (exp.is_odd()) {
                ans *= base;
            }
            base *= base;
            exp = exp / TWO;
        }
        return ans;
    }

    /**
    * @brief 幂运算，this^exp
    */
    Self pow(i64 exp) const {
        Self ans{ONE};
        Self base = *this;
        while (exp) {
            if (exp & 1) {
                ans *= base;
            }
            base *= base;
            exp >>= 1;
        }
        return ans;
    }

    /**
    * 截取 [low, high] 位到一个新数字，新数字与原数字符号相同
    * @param low 低位（包括），从1开始
    * @param high 高位（包括），从1开始
    * @return 新数字，若为非法截取，返回0
    */
    Self slice(usize low, usize high) const {
        if (low > high || low < 1 || high > this->size()) {
            return ZERO;
        }
        auto divisor = Self(10).pow(low - 1);
        auto modulus = Self(10).pow(high - low + 1);
        return (*this / divisor) % modulus;
    }

    /**
    * 截取从低位 low 开始的所有位到一个新数字，新数字与原数字符号相同
    * @param low 低位（包括），从1开始
    * @return 新数字，若为非法截取，返回0
    */
    Self slice(usize low) const {
        if (low < 1 || low > this->size()) {
            return ZERO;
        }
        auto divisor = Self(10).pow(low - 1);
        return *this / divisor;
    }

    /**
     * @brief 计算除法的商和余数
     * @param other 除数
     * @return 包含商和余数的pair
     * @throws throw runtime_exception 当除数为零时
     */
    Pair<BigInteger, BigInteger> div_rem(const Self& other) const {
        return Pair{*this / other, *this % other};
    }

    friend bool operator||(const BigInteger& a, const BigInteger& b) {
        return a != ZERO || b != ZERO;
    }

    friend bool operator&&(const BigInteger& a, const BigInteger& b) {
        return a != ZERO && b != ZERO;
    }

    bool operator!() const {
        return *this == ZERO;
    }

    explicit operator bool() const {
        return *this != ZERO;
    }

    /**
     * @brief 获取数值的字符串表示
     * @return 字符串形式的数值
     */
    [[nodiscard]] CString __str__() const {
        if (this->is_zero()) return "0";

        std::stringstream stream;
        if (!sign_) {
            stream << '-';
        }
        stream << num_.back();
        for (isize i = num_.size() - 2; i >= 0; --i) {
            stream << std::format("{:0{}d}", num_[i], WIDTH);
        }
        return CString{stream.str()};
    }

    [[nodiscard]] cmp_t __cmp__(const Self& other) const {
        if (this->is_pos() && other.is_neg()) {
            return 1;
        } else if (this->is_neg() && other.is_pos()) {
            return -1;
        } else if (this->is_pos() && other.is_pos()) {
            if (this->length_ < other.length_) {
                return -1;
            } else if (this->length_ > other.length_) {
                return 1;
            } else {
                auto a_size = this->num_.size();
                for (isize i = a_size - 1; i >= 0; --i) {
                    if (this->num_[i] < other.num_[i]) {
                        return -1;
                    } else if (this->num_[i] > other.num_[i]) {
                        return 1;
                    }
                }
                return 0;
            }
        } else {
            return other.abs().__cmp__(this->abs());
        }
    }

private:
    BigInteger(bool sign, usize length, util::Vec<i32>&& num) :
            sign_(sign), length_(length), num_(std::move(num)) {}

    /**
    * @brief 移除前导0
    */
    static void cut_leading_zero(util::Vec<i32>& num) {
        if (num.empty()) return;
        while (num.back() == 0 && num.size() != 1) {
            num.pop();
        }
    }

    /**
    * @brief 计算长度，每4字节存8个10进制位
    */
    void calc_len() {
        cut_leading_zero(num_);
        i32 tmp = num_.back();
        if (tmp == 0) {
            length_ = 1;
        } else {
            length_ = (num_.size() - 1) * WIDTH;
            while (tmp > 0) {
                ++length_;
                tmp /= 10;
            }
        }
    }

private:
    bool sign_;          ///< true=正数 false=负数
    usize length_;       ///< 十进制位数
    util::Vec<i32> num_; ///< 逆序存储，每4字节存WIDTH个10进制位

    static constexpr i32 BASE = 100000000; ///< 存储基数
    static constexpr i32 WIDTH = 8;        ///< 每个基本单元存储的十进制位数
};

inline const BigInteger BigInteger::ZERO{};
inline const BigInteger BigInteger::ONE{1};
inline const BigInteger BigInteger::TWO{2};
inline const BigInteger BigInteger::TEN{10};

} // namespace my::math

#endif // BIG_INTEGER_HPP