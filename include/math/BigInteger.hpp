/**
* @brief 高精度整数类
* @author Ricky
* @date 2025/1/6
* @version 1.0
*/
#ifndef BIG_INTEGER_HPP
#define BIG_INTEGER_HPP

#include "CString.hpp"
#include "Object.hpp"
#include "math_utils.hpp"

namespace my::math {

class BigInteger : public Object<BigInteger> {
    using self = BigInteger;

public:
    BigInteger(i64 num) {
        this->constructor(num);
    }

    BigInteger(const CString& num) {
        this->constructor(num);
    }

    BigInteger(const self& other) :
            negFlag_(other.negFlag_), bitCount_(other.bitCount_) {
        // TODO
    }

    i32 size() const {
        return this->bitCount_;
    }

    CString __str__() const {
        std::stringstream stream;
        stream << (negFlag_ ? "-" : "");
        for (int i = num_.size() - 1; i >= 0; --i) {
            stream << num_[i];
        }
        return CString{stream.str()};
    }

private:
    bool negFlag_; // 负数标记 true=负数 false=非负数
    i32 bitCount_; // 十进制位数
    CString num_;  // 逆序存储
private:
    i32 numSize(i64 num) {
        if (num == 0LL) return 1;
        if (num < 0LL) num = -num;
        return static_cast<i32>(std::floor(std::log10(num))) + 1;
    }

    self& constructor(i64 num) {
        this->negFlag_ = num < 0LL;
        this->bitCount_ = numSize(num);
        if (num == 0LL) {
            this->num_ = "0"_cs;
            return *this;
        }
        std::stringstream stream;
        num = std::abs(num);
        while (num) {
            stream << num % 10;
            num /= 10;
        }
        this->num_ = CString{stream.str()};
        return *this;
    }

    self& constructor(const CString& num) {
        c_size idx = 0, len = num.size();
        if (num[0] == '-') {
            this->negFlag_ = true;
            ++idx;
        } else {
            this->negFlag_ = false;
        }

        while (num[idx] == '0') {
            idx++;
        }
        this->bitCount_ = len - idx;
        if (this->bitCount_ == 0) {
            this->bitCount_ = 1;
            this->num_ = "0"_cs;
            return *this;
        }
        std::stringstream stream;
        for (int i = len - 1; i >= idx; --i) {
            stream << num[i];
        }
        this->num_ = CString{stream.str()};
        return *this;
    }
};

}; // namespace my::math

#endif // BIG_INTEGER_HPP