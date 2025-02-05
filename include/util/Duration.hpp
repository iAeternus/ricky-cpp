/**
 * @brief 时间间隔
 * @author Ricky
 * @date 2025/2/3
 * @version 2.0
 */
#ifndef DURATION_HPP
#define DURATION_HPP

#include "Object.hpp"
#include "math_utils.hpp"

namespace my::util {

class Duration : public Object<Duration> {
    using self = Duration;

public:
    const static self ZERO;

    static self ofDays(i64 days) {
        return self{math::multiplyExact(days, 86400LL), 0};
    }

    static self ofHours(i64 hours) {
        return self{math::multiplyExact(hours, 3600LL), 0};
    }

    static self ofMinutes(i64 minutes) {
        return self{math::multiplyExact(minutes, 60LL), 0};
    }

    static self ofSeconds(i64 seconds_, i32 nanos_ = 0) {
        validateNanos(nanos_);
        return self{seconds_, nanos_};
    }

    static self ofMillis(i64 millis) {
        i64 seconds = millis / 1000;
        i32 milliRem = static_cast<i32>(millis % 1000);
        if (milliRem < 0) {
            milliRem += 1000;
            seconds -= 1;
        }
        return self{seconds, milliRem * 1'000'000};
    }

    static self ofNanos(i64 nanos_) {
        i64 seconds = nanos_ / NANOS_PER_SECOND;
        i32 nanoRem = static_cast<i32>(nanos_ % NANOS_PER_SECOND);
        if (nanoRem < 0) {
            nanoRem += NANOS_PER_SECOND;
            seconds -= 1;
        }
        return self{seconds, nanoRem};
    }

    self operator+(const self& other) const {
        i64 sec = seconds_ + other.seconds_;
        i32 nano = nanos_ + other.nanos_;
        adjustCarry(sec, nano);
        return self{sec, nano};
    }

    self operator-(const self& other) const {
        i64 sec = seconds_ - other.seconds_;
        i32 nano = nanos_ - other.nanos_;
        adjustBorrow(sec, nano);
        return self{sec, nano};
    }

    self operator*(i64 scalar) const {
        i64 totalNanos = toNanos() * scalar;
        return ofNanos(totalNanos);
    }

    self operator/(i64 divisor) const {
        if (divisor == 0) {
            RuntimeError("Division by zero");
            return None<self>;
        }
        i64 totalNanos = toNanos();
        return ofNanos(totalNanos / divisor);
    }

    i64 toDays() const { return seconds_ / 86400; }
    i64 toHours() const { return seconds_ / 3600; }
    i64 toMinutes() const { return seconds_ / 60; }
    i64 getSeconds() const { return seconds_; }
    i32 getNanos() const { return nanos_; }

    cmp_t __cmp__(const self& other) const {
        if (seconds_ != other.seconds_)
            return seconds_ - other.seconds_;
        return nanos_ - other.nanos_;
    }

    CString __str__() const {
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
    i64 seconds_;
    i32 nanos_;
    static constexpr i32 NANOS_PER_SECOND = 1'000'000'000;

    Duration(i64 sec, i32 nano) :
            seconds_(sec), nanos_(nano) {
        validateNanos(nano);
    }

    static void validateNanos(i32 nanos_) {
        if (nanos_ < 0 || nanos_ >= NANOS_PER_SECOND)
            ValueError("Nanoseconds out of range");
    }

    /**
     * @brief 处理进位
     */
    static void adjustCarry(i64& sec, i32& nano) {
        if (nano >= NANOS_PER_SECOND) {
            nano -= NANOS_PER_SECOND;
            sec += 1;
        }
    }

    /**
     * @brief 处理借位
     */
    static void adjustBorrow(i64& sec, i32& nano) {
        if (nano < 0) {
            nano += NANOS_PER_SECOND;
            sec -= 1;
        }
    }

    i64 toNanos() const {
        return math::addExact(seconds_ * NANOS_PER_SECOND, nanos_);
    }
};

const Duration Duration::ZERO = Duration{0LL, 0};

} // namespace my::util

#endif // DURATION_HPP