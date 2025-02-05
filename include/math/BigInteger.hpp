/**
 * @brief 高精度整数类
 * @author Ricky
 * @date 2025/1/12
 * @version 1.0
 */
#ifndef BIG_INTEGER_HPP
#define BIG_INTEGER_HPP

#include "DynArray.hpp"

namespace my::math {

class BigInteger : public Object<BigInteger> {
    using self = BigInteger;

public:
    const static self ZERO;
    const static self ONE;

    BigInteger(i32 n = 0) {
        *this = n;
    }

    BigInteger(i64 n) {
        *this = n;
    }

    BigInteger(const char* n) {
        *this = n;
    }

    BigInteger(const CString& str) {
        *this = str;
    }

    BigInteger(const self& other) {
        *this = other;
    }

    self& operator=(i32 n) {
        *this = i64(n);
        return *this;
    }

    self& operator=(i64 n) {
        num_.clear();
        if (n == 0) {
            num_.append(0);
        }
        if (n >= 0) {
            sign_ = true;
        } else if (n == LONG_LONG_MIN) {
            *this = "9223372036854775808";
            this->sign_ = false;
            return *this;
        } else if (n < 0) {
            sign_ = false;
            n = -n;
        }
        while (n != 0) {
            num_.append(n % BASE);
            n /= BASE;
        }
        calcLength();
        return *this;
    }

    self& operator=(const char* str) {
        c_size len = strlen(str), stop = 0LL;
        i32 tmp = 0, ten = 1;
        num_.clear();
        sign_ = (str[0] != '-');
        if (!sign_) {
            stop = 1LL;
        }
        for (c_size i = len; i > stop; --i) {
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
        calcLength();
        return *this;
    }

    self& operator=(const CString& str) {
        *this = str.data();
        return *this;
    }

    self& operator=(const self& other) {
        this->sign_ = other.sign_;
        this->length_ = other.length_;
        this->num_ = other.num_;
        return *this;
    }

    c_size size() const {
        return length_;
    }

    /**
     * @brief 判断是否为奇数
     * @return true=是 false=否
     */
    bool isOdd() const {
        return num_.front() & 1;
    }

    /**
     * @brief 判断是否为偶数
     * @return true=是 false=否
     */
    bool isEven() const {
        return !isOdd();
    }

    /**
     * @brief 左移n位，低位补0
     */
    self leftShift(c_size n) const {
        c_size tmp = n % WIDTH;
        self ans;
        ans.length_ = n + 1;
        n /= WIDTH;
        while (ans.num_.size() <= n) {
            ans.num_.append(0);
        }
        ans.num_[n] = 1;
        while (tmp--) {
            ans.num_[n] *= 10;
        }
        return ans * (*this);
    }

    self abs() const {
        self ans{*this};
        ans.sign_ = true;
        return ans;
    }

    const self& operator+() const {
        return *this;
    }

    friend self operator+(const self& a, const self& b) {
        if (!b.sign_) {
            return a - (-b);
        }
        if (!a.sign_) {
            return b - (-a);
        }
        self ans;
        i32 carry = 0, aa, bb;
        c_size aSize = a.num_.size(), bSize = b.num_.size();
        c_size maxSize = std::max(aSize, bSize);
        ans.num_.clear();
        for (c_size i = 0; i < maxSize; ++i) {
            aa = aSize <= i ? 0 : a.num_[i];
            bb = bSize <= i ? 0 : b.num_[i];
            ans.num_.append((aa + bb + carry) % BASE);
            carry = (aa + bb + carry) / BASE;
        }
        if (carry > 0) {
            ans.num_.append(carry);
        }
        ans.calcLength();
        return ans;
    }

    self& operator+=(const self& other) {
        *this = *this + other;
        return *this;
    }

    self& operator++() {
        *this = *this + 1;
        return *this;
    }

    self operator++(int) {
        self ans{*this};
        ++*this;
        return ans;
    }

    self operator-() const {
        self ans{*this};
        if (ans != 0) {
            ans.sign_ = !ans.sign_;
        }
        return ans;
    }

    friend self operator-(const self& a, const self& b) {
        if (!b.sign_) {
            return a + (-b);
        }
        if (!a.sign_) {
            return -((-a) + b);
        }
        if (a < b) {
            return -(b - a);
        }
        self ans;
        i32 carry = 0, aa, bb;
        c_size aSize = a.num_.size(), bSize = b.num_.size();
        c_size maxSize = std::max(aSize, bSize);
        ans.num_.clear();
        for (c_size i = 0; i < maxSize; ++i) {
            aa = a.num_[i];
            bb = bSize <= i ? 0 : b.num_[i];
            ans.num_.append((aa - bb - carry + BASE) % BASE);
            carry = aa < bb + carry ? 1 : 0;
        }
        ans.calcLength();
        return ans;
    }

    self& operator-=(const self& other) {
        *this = *this - other;
        return *this;
    }

    self& operator--() {
        *this = *this - 1;
        return *this;
    }

    self operator--(int) {
        self ans{*this};
        --*this;
        return ans;
    }

    friend self operator*(const self& a, const self& b) {
        c_size aSize = a.num_.size(), bSize = b.num_.size();
        util::DynArray<i64> res;
        for (c_size i = 0; i < aSize; ++i) {
            for (c_size j = 0; j < bSize; ++j) {
                i64 tmp = i64(a.num_[i]) * i64(b.num_[j]);
                i + j < res.size() ? res[i + j] += tmp : res.append(tmp);
            }
        }
        while (res.back() == 0 && res.size() != 1) {
            res.pop();
        }
        self ans;
        ans.sign_ = a.sign_ == b.sign_ || (res.size() == 1 && res[0] == 0);
        ans.num_.clear();
        c_size resSize = res.size();
        i64 carry = 0, tmp;
        for (c_size i = 0; i < resSize; ++i) {
            tmp = res[i];
            ans.num_.append((tmp + carry) % BASE);
            carry = (tmp + carry) / BASE;
        }
        if (carry > 0) {
            ans.num_.append(carry);
        }
        ans.calcLength();
        return ans;
    }

    self& operator*=(const self& other) {
        *this = *this * other;
        return *this;
    }

    friend self operator/(const self& a, const self& b) {
        if (b == 0) {
            ValueError("Divide by 0");
        }
        self aa{a.abs()}, bb{b.abs()};
        if (aa < bb) {
            return 0;
        }
        CString str(aa.size() + 1);
        std::memset(str.data(), 0, str.size());
        self tmp;
        c_size lenDiff = aa.length_ - bb.length_;
        for (c_size i = 0; i <= lenDiff; ++i) {
            tmp = bb.leftShift(lenDiff - i);
            while (aa >= tmp) {
                ++str[i];
                aa -= tmp;
            }
            str[i] = i2c(str[i]);
        }
        self ans{str};
        ans.sign_ = a.sign_ == b.sign_ || ans == 0;
        return ans;
    }

    self operator/=(const self& other) {
        *this = *this / other;
        return *this;
    }

    friend self operator%(const self& a, const self& b) {
        return a - a / b * b;
    }

    self& operator%=(const self& other) {
        *this = *this % other;
        return *this;
    }

    /**
     * @brief 幂运算，base^exp
     */
    friend self operator^(self base, self exp) {
        self ans{ONE};
        for (; exp; exp /= 2, base *= base) {
            if (exp.isOdd()) {
                ans *= base;
            }
        }
        return ans;
    }

    friend bool operator==(const self& a, const self& b) {
        return a.__equals__(b);
    }

    friend bool operator!=(const self& a, const self& b) {
        return !a.__equals__(b);
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

    CString __str__() const {
        c_size mSize = num_.size();
        std::stringstream stream;
        if (!sign_) {
            stream << '-';
        }
        stream << num_.back();
        for (c_size i = mSize - 2; i >= 0; --i) {
            // 改WIDTH的时候这里要改
            stream << std::format("{:08d}", num_[i]);
        }
        return CString{stream.str()};
    }

    cmp_t __cmp__(const self& other) const {
        if (this->sign_ && !other.sign_) {
            return 1;
        } else if (!this->sign_ && other.sign_) {
            return -1;
        } else if (this->sign_ && other.sign_) {
            if (this->length_ < other.length_) {
                return -1;
            } else if (this->length_ > other.length_) {
                return 1;
            } else {
                c_size aSize = this->num_.size();
                for (c_size i = aSize - 1; i >= 0; --i) {
                    if (this->num_[i] < other.num_[i]) {
                        return -1;
                    } else if (this->num_[i] > other.num_[i]) {
                        return 1;
                    }
                }
                return 0;
            }
        } else {
            return -other.__cmp__(-*this);
        }
    }

private:
    /**
     * @brief 移除前导0
     */
    void cutLeadingZero() {
        while (num_.back() == 0 && num_.size() != 1) {
            num_.pop();
        }
    }

    /**
     * @brief 计算长度，每4字节存8个10进制位
     */
    void calcLength() {
        cutLeadingZero();
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
    bool sign_;               // true=正数 false=负数
    c_size length_;           // 十进制位数
    util::DynArray<i32> num_; // 逆序存储，每4字节存WIDTH个10进制位

    static const i32 BASE = 100000000;
    static const i32 WIDTH = 8;
};

const BigInteger BigInteger::ZERO = 0;
const BigInteger BigInteger::ONE = 1;

} // namespace my::math

#endif // BIG_INTEGER_HPP