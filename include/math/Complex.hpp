/**
 * @brief 复数
 * @author Ricky
 * @date 2025/1/13
 * @version 1.0
 */
#ifndef COMPLEX_HPP
#define COMPLEX_HPP

#include "math_utils.hpp"

namespace my::math {

class Complex : public Object<Complex> {
public:
    using Self = Complex;
    const static Self ZERO;

    Complex(f64 re_ = 0.0, f64 im_ = 0.0) :
            re_(re_), im_(im_) {}

    Complex(const char* str) {
        *this = str;
    }

    Self& operator=(const char* str) {
        parse(str);
        return *this;
    }

    f64 real() const {
        return re_;
    }

    f64 imag() const {
        return im_;
    }

    f64 modulus_sqr() const {
        return re_ * re_ + im_ * im_;
    }

    /**
     * @brief 计算模长
     */
    f64 modulus() const {
        return std::sqrt(modulus_sqr());
    }

    /**
     * @brief 计算俯角，单位：弧度
     */
    f64 angle() const {
        return std::atan2(im_, re_);
    }

    /**
     * @brief 共轭复数
     */
    Self conj() const {
        return Self{re_, -im_};
    }

    friend Self operator+(const Self& a, const Self& b) {
        return Self{a.re_ + b.re_, a.im_ + b.im_};
    }

    Self& operator+=(const Self& other) {
        *this = *this + other;
        return *this;
    }

    friend Self operator-(const Self& a, const Self& b) {
        return Self{a.re_ - b.re_, a.im_ - b.im_};
    }

    Self& operator-=(const Self& other) {
        *this = *this - other;
        return *this;
    }

    friend Self operator*(const Self& a, const Self& b) {
        return Self{a.re_ * b.re_ - a.im_ * b.im_,
                    a.im_ * b.re_ + a.re_ * b.im_};
    }

    Self& operator*=(const Self& other) {
        *this = *this * other;
        return *this;
    }

    friend Self operator/(const Self& a, const Self& b) {
        auto bNormSqr = b.modulus_sqr();
        return Self{(a.re_ * b.re_ + a.im_ * b.im_) / bNormSqr,
                    (a.im_ * b.re_ - a.re_ * b.im_) / bNormSqr};
    }

    Self& operator/=(const Self& other) {
        *this = *this / other;
        return *this;
    }

    [[nodiscard]] CString __str__() const {
        std::stringstream stream;
        if (is_zero(re_) && is_zero(im_)) {
            return CString{"0"};
        }

        if (!is_zero(re_)) {
            stream << re_;
        }
        if (!is_zero(im_)) {
            if (!is_zero(re_) && is_pos(im_)) {
                stream << '+';
            }
            if (!is_one(im_)) {
                stream << im_;
            }
            stream << 'i';
        }
        return CString{stream.str()};
    }

    [[nodiscard]] cmp_t __cmp__(const Self& other) const {
        (void)other;
        throw runtime_exception("Complex numbers cannot compare sizes");
    }

    [[nodiscard]] bool __equals__(const Self& other) const {
        return fcmp(this->re_, other.re_) == 0 && fcmp(this->im_, other.im_) == 0;
    }

private:
    void parse(const CString& str) {
        usize i = 0;
        auto trim_str = str.remove_all(' ');
        auto t_size = trim_str.size();
        if (i < t_size && trim_str[i] == '+') {
            ++i;
            re_ = parse_num(trim_str, i);
        } else if (i < t_size && trim_str[i] == '-') {
            ++i;
            re_ = -parse_num(trim_str, i);
        } else {
            re_ = peek_real(trim_str) ? parse_num(trim_str, i) : 0.0;
        }

        if (i < trim_str.size() && trim_str[i] == '+') {
            ++i;
            im_ = parse_num(trim_str, i);
        } else if (i < trim_str.size() && trim_str[i] == '-') {
            ++i;
            im_ = -parse_num(trim_str, i);
        } else {
            im_ = peek_imag(trim_str, i) ? parse_num(trim_str, i) : 0.0;
        }
        if (i < trim_str.size() && is_imag_sign(trim_str[i])) {
            ++i;
        }
        if (i != trim_str.size()) {
            throw runtime_exception("Invalid complex number format");
        }
    }

    static bool is_imag_sign(char ch) {
        return ch == 'i' || ch == 'I';
    }

    static f64 parse_num(const CString& str, usize& i) {
        if (is_imag_sign(str[i])) {
            return 1.0;
        }
        std::string num;
        while (i < str.size() && (std::isdigit(str[i]) || str[i] == '.')) {
            num += str[i++];
        }
        if (num.empty()) {
            throw runtime_exception("Invalid complex number format");
        }
        return std::stod(num);
    }

    /**
     * 从第一个数字起探测是否有实部，不会检查字符是否是数字
     */
    static bool peek_real(const CString& str) {
        auto m_size = str.size();
        for (usize i = 0; i < m_size; ++i) {
            if (str[i] == '+' || str[i] == '-') {
                return true;
            } else if (is_imag_sign(str[i])) {
                return false;
            }
        }
        return true;
    }

    /**
     * 从i起探测是否有虚部，不会检查字符是否是数字
     */
    static bool peek_imag(const CString& str, usize i) {
        auto m_size = str.size();
        for (usize j = i; j < m_size; ++j) {
            if (is_imag_sign(str[j])) {
                return true;
            }
        }
        return false;
    }

private:
    f64 re_; // 实部
    f64 im_; // 虚部
};

inline const Complex Complex::ZERO{};

} // namespace my::math

#endif // COMPLEX_HPP