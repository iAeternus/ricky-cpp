/**
* @brief 复数
* @author Ricky
* @date 2025/1/13
* @version 1.0
*/
#ifndef COMPLEX_HPP
#define COMPLEX_HPP

#include "math_utils.hpp"
#include "String.hpp"

namespace my::math {

class Complex : public Object<Complex> {
    using self = Complex;

public:
    const static self ZERO;

    Complex(f64 real_ = 0.0, f64 imag_ = 0.0) :
            real_(real_), imag_(imag_) {}

    Complex(const char* str) {
        *this = str;
    }

    self& operator=(const char* str) {
        parse(util::String{str});
        return *this;
    }

    f64 real() const {
        return real_;
    }

    f64 imag() const {
        return imag_;
    }

    f64 normSqr() const {
        return real_ * real_ + imag_ * imag_;
    }

    /**
    * @brief 计算模长
    */
    f64 norm() const {
        return std::sqrt(normSqr());
    }

    /**
     * @brief 计算俯角，单位：弧度
     */
    f64 arg() const {
        return std::atan2(imag_, real_);
    }

    /**
    * @brief 共轭复数
    */
    self conj() const {
        return self{real_, -imag_};
    }

    friend self operator+(const self& a, const self& b) {
        return self{a.real_ + b.real_, a.imag_ + b.imag_};
    }

    self& operator+=(const self& other) {
        *this = *this + other;
        return *this;
    }

    friend self operator-(const self& a, const self& b) {
        return self{a.real_ - b.real_, a.imag_ - b.imag_};
    }

    self& operator-=(const self& other) {
        *this = *this - other;
        return *this;
    }

    friend self operator*(const self& a, const self& b) {
        return self{a.real_ * b.real_ - a.imag_ * b.imag_,
                    a.imag_ * b.real_ + a.real_ * b.imag_};
    }

    self& operator*=(const self& other) {
        *this = *this * other;
        return *this;
    }

    friend self operator/(const self& a, const self& b) {
        f64 bNormSqr = b.normSqr();
        return self{(a.real_ * b.real_ + a.imag_ * b.imag_) / bNormSqr,
                    (a.imag_ * b.real_ - a.real_ * b.imag_) / bNormSqr};
    }

    self& operator/=(const self& other) {
        *this = *this / other;
        return *this;
    }

    CString __str__() const {
        std::stringstream stream;
        if (isZero(real_) && isZero(imag_)) {
            return CString{"0"};
        }

        if (!isZero(real_)) {
            stream << real_;
        }
        if (!isZero(imag_)) {
            if (!isZero(real_) && isPositive(imag_)) {
                stream << '+';
            }
            if (!isOne(imag_)) {
                stream << imag_;
            }
            stream << 'i';
        }
        return CString{stream.str()};
    }

    cmp_t __cmp__(const self& other) const {
        (void)other;
        RuntimeError("Complex numbers cannot compare sizes");
        return None<cmp_t>;
    }

    bool __equals__(const self& other) const {
        return compare(this->real_, other.real_) == 0 && compare(this->imag_, other.imag_) == 0;
    }

private:
    void parse(const util::String& str) {
        c_size i = 0LL;
        auto trimStr = str.removeAll(' ');
        c_size tSize = trimStr.size();
        if (i < tSize && trimStr[i] == '+') {
            ++i;
            real_ = parseNumber(trimStr, i);
        } else if (i < tSize && trimStr[i] == '-') {
            ++i;
            real_ = -parseNumber(trimStr, i);
        } else {
            real_ = peekReal(trimStr) ? parseNumber(trimStr, i) : 0.0;
        }

        if (i < trimStr.size() && trimStr[i] == '+') {
            ++i;
            imag_ = parseNumber(trimStr, i);
        } else if (i < trimStr.size() && trimStr[i] == '-') {
            ++i;
            imag_ = -parseNumber(trimStr, i);
        } else {
            imag_ = peekImag(trimStr, i) ? parseNumber(trimStr, i) : 0.0;
        }
        if (i < trimStr.size() && isImagSign(trimStr[i])) {
            ++i;
        }
        if (i != trimStr.size()) {
            ValueError("Invalid complex number format");
        }
    }

    static bool isImagSign(const util::CodePoint& ch) {
        return ch == 'i' || ch == 'I';
    }

    static f64 parseNumber(const util::String& str, c_size& i) {
        if (isImagSign(str[i])) {
            return 1.0;
        }
        std::string number;
        while (i < str.size() && (std::isdigit(str[i]) || str[i] == '.')) {
            number += str[i++];
        }
        if (number.empty()) {
            ValueError("Invalid number format");
            return None<f64>;
        }
        return std::stod(number);
    }

    /**
    * 从第一个数字起探测是否有实部，不会检查字符是否是数字
    */
    static bool peekReal(const util::String& str) {
        c_size mSize = str.size();
        for (c_size i = 0; i < mSize; ++i) {
            if (str[i] == '+' || str[i] == '-') {
                return true;
            } else if (isImagSign(str[i])) {
                return false;
            }
        }
        return true;
    }

    /**
    * 从i起探测是否有虚部，不会检查字符是否是数字
    */
    static bool peekImag(const util::String& str, c_size i) {
        c_size mSize = str.size();
        for (c_size j = i; j < mSize; ++j) {
            if (isImagSign(str[j])) {
                return true;
            }
        }
        return false;
    }

private:
    f64 real_; // 实部
    f64 imag_; // 虚部
};

} // namespace my::math

#endif // COMPLEX_HPP