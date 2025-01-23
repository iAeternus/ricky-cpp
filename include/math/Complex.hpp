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

    Complex(f64 re_ = 0.0, f64 im_ = 0.0) :
            re_(re_), im_(im_) {}

    Complex(const char* str) {
        *this = str;
    }

    self& operator=(const char* str) {
        parse(util::String{str});
        return *this;
    }

    f64 real() const {
        return re_;
    }

    f64 imag() const {
        return im_;
    }

    f64 normSqr() const {
        return re_ * re_ + im_ * im_;
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
        return std::atan2(im_, re_);
    }

    /**
    * @brief 共轭复数
    */
    self conj() const {
        return self{re_, -im_};
    }

    friend self operator+(const self& a, const self& b) {
        return self{a.re_ + b.re_, a.im_ + b.im_};
    }

    self& operator+=(const self& other) {
        *this = *this + other;
        return *this;
    }

    friend self operator-(const self& a, const self& b) {
        return self{a.re_ - b.re_, a.im_ - b.im_};
    }

    self& operator-=(const self& other) {
        *this = *this - other;
        return *this;
    }

    friend self operator*(const self& a, const self& b) {
        return self{a.re_ * b.re_ - a.im_ * b.im_,
                    a.im_ * b.re_ + a.re_ * b.im_};
    }

    self& operator*=(const self& other) {
        *this = *this * other;
        return *this;
    }

    friend self operator/(const self& a, const self& b) {
        f64 bNormSqr = b.normSqr();
        return self{(a.re_ * b.re_ + a.im_ * b.im_) / bNormSqr,
                    (a.im_ * b.re_ - a.re_ * b.im_) / bNormSqr};
    }

    self& operator/=(const self& other) {
        *this = *this / other;
        return *this;
    }

    CString __str__() const {
        std::stringstream stream;
        if (isZero(re_) && isZero(im_)) {
            return CString{"0"};
        }

        if (!isZero(re_)) {
            stream << re_;
        }
        if (!isZero(im_)) {
            if (!isZero(re_) && isPositive(im_)) {
                stream << '+';
            }
            if (!isOne(im_)) {
                stream << im_;
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
        return compare(this->re_, other.re_) == 0 && compare(this->im_, other.im_) == 0;
    }

private:
    void parse(const util::String& str) {
        c_size i = 0LL;
        auto trimStr = str.removeAll(' ');
        c_size tSize = trimStr.size();
        if (i < tSize && trimStr[i] == '+') {
            ++i;
            re_ = parseNumber(trimStr, i);
        } else if (i < tSize && trimStr[i] == '-') {
            ++i;
            re_ = -parseNumber(trimStr, i);
        } else {
            re_ = peekReal(trimStr) ? parseNumber(trimStr, i) : 0.0;
        }

        if (i < trimStr.size() && trimStr[i] == '+') {
            ++i;
            im_ = parseNumber(trimStr, i);
        } else if (i < trimStr.size() && trimStr[i] == '-') {
            ++i;
            im_ = -parseNumber(trimStr, i);
        } else {
            im_ = peekImag(trimStr, i) ? parseNumber(trimStr, i) : 0.0;
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
    f64 re_; // 实部
    f64 im_; // 虚部
};

} // namespace my::math

#endif // COMPLEX_HPP