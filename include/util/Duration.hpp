/**
 * @brief 时间间隔
 * @note 此文件参考java，故使用java命名风格
 * @author Ricky
 * @date 2025/2/3
 * @version 2.0
 */
#ifndef DURATION_HPP
#define DURATION_HPP

#include "Exception.hpp"
#include "math_utils.hpp"

namespace my::util {

/**
 * @brief 时间间隔类，表示时间的长短。
 *        支持多种时间单位的创建、运算、转换和比较。
 */
class Duration : public Object<Duration> {
    using Self = Duration;

public:
    const static Self ZERO; // 零时间间隔的常量对象

    /**
     * @brief 创建以天为单位的时间间隔对象。
     * @param days 天数
     * @return 时间间隔对象
     * @note 不允许传入受不同类型溢出影响的值
     * @example Duration::ofDays(5) 创建 5 天的时间间隔
     */
    static Self ofDays(i64 days) {
        return Self{math::mul_exact(days, 86400LL), 0};
    }

    /**
     * @brief 创建以小时为单位的时间间隔对象。
     * @param hours 小时数
     * @return 时间间隔对象
     * @note 不允许传入受不同类型溢出影响的值
     * @example Duration::ofHours(3) 创建 3 小时的时间间隔
     */
    static Self ofHours(i64 hours) {
        return Self{math::mul_exact(hours, 3600LL), 0};
    }

    /**
     * @brief 创建以分钟为单位的时间间隔对象。
     * @param minutes 分钟数
     * @return 时间间隔对象
     * @note 不允许传入受不同类型溢出影响的值
     * @example Duration::ofMinutes(45) 创建 45 分钟的时间间隔
     */
    static Self ofMinutes(i64 minutes) {
        return Self{math::mul_exact(minutes, 60LL), 0};
    }

    /**
     * @brief 创建以秒和纳秒为单位的时间间隔对象。
     * @param seconds 秒数
     * @param nanos_ 纳秒数（0-999,999,999）
     * @return 时间间隔对象
     * @exception ValueError 若纳秒超出范围
     * @example Duration::ofSeconds(10, 500000000) 创建 10.5 秒的时间间隔
     */
    static Self ofSeconds(i64 seconds_, i32 nanos_ = 0) {
        validateNanos(nanos_);
        return Self{seconds_, nanos_};
    }

    /**
     * @brief 创建以毫秒为单位的时间间隔对象。
     * @param millis 毫秒数
     * @return 时间间隔对象
     * @note 负数的处理可能正确，也可能不正确
     * @example Duration::ofMillis(1500) 创建 1.5 秒的时间间隔
     */
    static Self ofMillis(i64 millis) {
        i64 seconds = millis / 1000;
        i32 milliRem = static_cast<i32>(millis % 1000);
        if (milliRem < 0) {
            milliRem += 1000;
            seconds -= 1;
        }
        return Self{seconds, milliRem * 1'000'000};
    }

    /**
     * @brief 创建以纳秒为单位的时间间隔对象。
     * @param nanos_ 纳秒数
     * @return 时间间隔对象
     * @exception ValueError 若纳秒超出范围
     * @example Duration::ofNanos(500000000) 创建 0.5 秒的时间间隔
     */
    static Self ofNanos(i64 nanos_) {
        i64 seconds = nanos_ / NANOS_PER_SECOND;
        i32 nanoRem = static_cast<i32>(nanos_ % NANOS_PER_SECOND);
        if (nanoRem < 0) {
            nanoRem += NANOS_PER_SECOND;
            seconds -= 1;
        }
        return Self{seconds, nanoRem};
    }

    /**
     * @brief 时间间隔加法运算。
     * @param other 另一个时间间隔对象
     * @return 加法结果的时间间隔对象（*this + other）
     */
    Self operator+(const Self& other) const {
        i64 sec = seconds_ + other.seconds_;
        i32 nano = nanos_ + other.nanos_;
        adjustCarry(sec, nano);
        return Self{sec, nano};
    }

    /**
     * @brief 时间间隔减法运算。
     * @param other 另一个时间间隔对象
     * @return 减法结果的时间间隔对象（*this - other）
     */
    Self operator-(const Self& other) const {
        i64 sec = seconds_ - other.seconds_;
        i32 nano = nanos_ - other.nanos_;
        adjustBorrow(sec, nano);
        return Self{sec, nano};
    }

    /**
     * @brief 时间间隔乘法运算。
     * @param scalar 标量
     * @return 乘法结果的时间间隔对象（*this * scalar）
     */
    Self operator*(i64 scalar) const {
        i64 totalNanos = toNanos() * scalar;
        return ofNanos(totalNanos);
    }

    /**
     * @brief 时间间隔除法运算。
     * @param divisor 除数
     * @return 除法结果的时间间隔对象（*this / divisor）
     * @exception throw arithmetic_exception 除数为零
     */
    Self operator/(i64 divisor) const {
        if (divisor == 0) {
            throw arithmetic_exception("/ by zero");
        }
        i64 totalNanos = toNanos();
        return ofNanos(totalNanos / divisor);
    }

    /**
     * @brief 获取时间间隔的天数部分。
     * @return 天数
     * @example 5 天 3 小时的时间间隔返回 5
     */
    i64 toDays() const { return seconds_ / 86400; }

    /**
     * @brief 获取时间间隔的小时部分。
     * @return 小时数
     * @example 3 小时的时间间隔返回 3
     */
    i64 toHours() const { return seconds_ / 3600; }

    /**
     * @brief 获取时间间隔的分钟部分。
     * @return 分钟数
     * @example 45 分钟的时间间隔返回 45
     */
    i64 toMinutes() const { return seconds_ / 60; }

    /**
     * @brief 获取时间间隔的秒数。
     * @return 秒数
     * @note 等同于 getSeconds()
     */
    i64 getSeconds() const { return seconds_; }

    /**
     * @brief 获取时间间隔的纳秒数。
     * @return 纳秒数（0-999,999,999）
     */
    i32 getNanos() const { return nanos_; }

    /**
     * @brief 比较两个时间间隔对象。
     * @param other 另一个时间间隔对象
     * @return 比较结果（-1、0、1），0 表示相等
     */
    [[nodiscard]] cmp_t __cmp__(const Self& other) const {
        if (seconds_ != other.seconds_)
            return seconds_ - other.seconds_;
        return nanos_ - other.nanos_;
    }

    /**
     * @brief 返回时间间隔的字符串表示（如 PT1H30M30.0S）。
     * @return 时间间隔的字符串
     */
    [[nodiscard]] CString __str__() const {
        if (*this == ZERO) {
            return CString{"PT0.0S"};
        }

        std::stringstream stream;
        stream << "PT";
        i64 days = toDays();
        if (days != 0) stream << days << 'D';
        i64 hours = (seconds_ / 3600) % 24;
        if (hours != 0) stream << hours << 'H';
        i64 minutes = (seconds_ / 60) % 60;
        if (minutes != 0) stream << minutes << 'M';
        i64 secs = seconds_ % 60;
        if (secs != 0 || nanos_ != 0) {
            stream << secs;
            if (nanos_ != 0) {
                stream << std::format(".{:09}", nanos_);
            }
            stream << 'S';
        }
        return CString(stream.str());
    }

private:
    /**
     * @brief 私有构造函数，用于创建时间间隔对象。
     * @param sec 秒数
     * @param nano 纳秒数
     */
    Duration(i64 sec, i32 nano) :
            seconds_(sec), nanos_(nano) {
        validateNanos(nano);
    }

    /**
     * @brief 验证纳秒数是否有效。
     * @param nanos_ 纳秒数
     * @exception ValueError 纳秒数超出范围
     */
    static void validateNanos(i32 nanos_) {
        if (nanos_ < 0 || nanos_ >= NANOS_PER_SECOND)
            throw runtime_exception("nanoseconds out of range");
    }

    /**
     * @brief 调整进位。
     * @param sec 秒数
     * @param nano 纳秒数
     * @note 当纳秒数大于等于 1 秒时，调整为秒数和余数
     */
    static void adjustCarry(i64& sec, i32& nano) {
        if (nano >= NANOS_PER_SECOND) {
            nano -= NANOS_PER_SECOND;
            sec += 1;
        }
    }

    /**
     * @brief 调整借位。
     * @param sec 秒数
     * @param nano 纳秒数
     * @note 当纳秒数小于 0 时，调整为秒数和余数
     */
    static void adjustBorrow(i64& sec, i32& nano) {
        if (nano < 0) {
            nano += NANOS_PER_SECOND;
            sec -= 1;
        }
    }

    /**
     * @brief 将时间间隔转换为纳秒数。
     * @return 总纳秒数
     */
    i64 toNanos() const {
        return math::add_exact(seconds_ * NANOS_PER_SECOND, nanos_);
    }

private:
    i64 seconds_; // 秒数
    i32 nanos_;   // 纳秒数（0-999,999,999）

    // 纳秒数每秒的纳秒数
    static constexpr i32 NANOS_PER_SECOND = 1'000'000'000;
};

inline const Duration Duration::ZERO = Duration{0LL, 0};

} // namespace my::util

#endif // DURATION_HPP