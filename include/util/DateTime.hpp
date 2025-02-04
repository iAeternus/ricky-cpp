/**
 * @brief 日期时间，描述时间点/时间刻
 * @author Ricky
 * @date 2025/2/3
 * @version 1.0
 */
#ifndef DATE_TIME_HPP
#define DATE_TIME_HPP

#include "Duration.hpp"

#include <ctime>
#include <chrono>
#include <algorithm>

namespace my::util {

/**
 * @brief 日期
 */
class Date : public Object<Date> {
    using self = Date;

public:
    static const self MIN;   // -999999999-01-01
    static const self MAX;   // 999999999-12-31
    static const self EPOCH; // 1970-01-01

    static self of(i32 year, i32 month = 1, i32 dayOfMonth = 1) {
        validate(year, month, dayOfMonth);
        return self{year, month, dayOfMonth};
    }

    static self ofYearDay(i32 year, i32 dayOfYear) {
        validateYear(year);
        const bool isLeap = isLeapYear(year);
        if (dayOfYear < 1 || dayOfYear > (isLeap ? 366 : 365)) {
            ValueError("Day of year out of range");
            return None<self>;
        }

        static constexpr i32 leap[] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335};
        static constexpr i32 common[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
        const auto& offsets = isLeap ? leap : common;

        i32 month = static_cast<i32>(std::lower_bound(std::begin(offsets), std::end(offsets), dayOfYear) - offsets) - 1;
        i32 day = dayOfYear - offsets[month];
        return self{year, month + 1, day};
    }

    static self fromEpochDay(i64 epochDay) {
        return fromEpochDayImpl(epochDay);
    }

    /**
     * @brief 解析"yyyy-MM-dd"格式
     */
    static self parse(const CString& str) {
        i32 year, month, day;
        if (sscanf(str.data(), "%d-%d-%d", &year, &month, &day) != 3) {
            ValueError("Invalid date format");
        }
        return self::of(year, month, day);
    }

    /**
     * @brief 获取当前日期（UTC时间），线程不安全
     */
    static self now() {
        auto now = std::chrono::system_clock::now();
        time_t time = std::chrono::system_clock::to_time_t(now);
        auto time_tm = localtime(&time);
        return self{time_tm->tm_year + 1900, time_tm->tm_mon + 1, time_tm->tm_mday};
    }

    i32 year() const noexcept { return year_; }
    i32 month() const noexcept { return month_; }
    i32 day() const noexcept { return day_; }

    i32 dayOfYear() const {
        static constexpr i32 leap[] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335};
        static constexpr i32 common[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
        return (isLeapYear() ? leap : common)[month_ - 1] + day_;
    }

    /**
     * @brief 计算星期
     * @return [1, 7]
     */
    i32 dayOfWeek() const noexcept {
        i64 epochDay = toEpochDay();
        return i32(epochDay >= -4 ? (epochDay + 4) % 7 : (epochDay + 5) % 7 + 6);
    }

    /**
     * @brief 判断闰年
     */
    bool isLeapYear() const noexcept {
        return isLeapYear(year_);
    }

    /**
     * @brief 日期运算
     */
    self plusDays(i64 days) const {
        return fromEpochDay(toEpochDay() + days);
    }

    self plusMonths(i64 months) const {
        i64 total = year_ * 12LL + (month_ - 1) + months;
        i64 newYear = total / 12;
        i32 newMonth = total % 12 + 1;
        if (newMonth < 1) {
            newMonth += 12;
            newYear -= 1;
        }
        return adjustDay(newYear, newMonth);
    }

    self plusYears(i64 years) const {
        return adjustDay(year_ + years, month_);
    }

    self minusDays(i64 days) const {
        return plusDays(-days);
    }

    self minusMonths(i64 months) const {
        return plusMonths(-months);
    }

    self minusYears(i64 years) const {
        return plusYears(-years);
    }

    /**
     * @brief 日期调整
     */
    self withYear(i32 year) const { return adjustDay(year, month_); }
    self withMonth(i32 month) const { return adjustDay(year_, month); }
    self withDay(i32 day) const { return self::of(year_, month_, day); }

    /**
     * @brief 本月的最后一天
     */
    self lastDayOfMonth() const {
        return self{year_, month_, daysInMonth(year_, month_)};
    }

    Duration operator-(const self& other) const {
        return Duration::ofDays(toEpochDay() - other.toEpochDay());
    }

    /**
     * @brief 转换为纪元日，第0天为1970年1月1日
     */
    i64 toEpochDay() const {
        i32 y = year_ - (month_ <= 2);
        i32 m = month_;
        i32 d = day_;
        i64 era = (y >= 0 ? y : y - 399) / 400;
        u32 yoe = y - era * 400;
        u32 doy = (153 * (m > 2 ? m - 3 : m + 9) + 2) / 5 + d - 1;
        u32 doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
        return era * 146097 + doe - 719468;
    }

    cmp_t __cmp__(const self& other) const {
        return this->year_ != other.year_   ? this->year_ - other.year_ :
               this->month_ != other.month_ ? this->month_ - other.month_ :
                                              this->day_ - other.day_;
    }

    CString __str__() const {
        char buf[11];
        snprintf(buf, sizeof(buf), "%04d-%02d-%02d", year_, month_, day_);
        return CString{buf};
    }

private:
    i32 year_;  // [-999999999, 999999999]
    i32 month_; // [1,12]
    i32 day_;   // [1,31]

    Date(i32 year, i32 month, i32 day) :
            year_(year), month_(month), day_(day) {}

    static self fromEpochDayImpl(i64 epochDay) {
        i64 z = epochDay + 719468;
        i64 era = (z >= 0 ? z : z - 146096) / 146097;
        u32 doe = static_cast<u32>(z - era * 146097);
        u32 yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
        i64 y = yoe + era * 400;
        u32 doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
        u32 mp = (5 * doy + 2) / 153;
        u32 d = doy - (153 * mp + 2) / 5 + 1;
        u32 m = mp < 10 ? mp + 3 : mp - 9;
        y += (m <= 2);
        return self{i32(y), i32(m), i32(d)};
    }

    /**
     * @brief 调整到有效日期
     */
    self adjustDay(i64 newYear, i32 newMonth) const {
        newYear = clampYear(newYear);
        newMonth = clampMonth(newMonth);
        i32 newDay = math::min(day_, daysInMonth(newYear, newMonth));
        return self{static_cast<i32>(newYear), newMonth, newDay};
    }

    static i64 clampYear(i64 year) {
        if (year < -999999999 || year > 999999999) {
            ValueError("Year overflow");
            return None<i64>;
        }
        return year;
    }

    static i32 clampMonth(i32 month) {
        if (month < 1 || month > 12) {
            ValueError("Invalid month");
            return None<i32>;
        }
        return month;
    }

    /**
     * @brief 校验合法性
     */
    static void validateYear(i32 year) {
        if (year < -999999999 || year > 999999999) ValueError("Year out of range");
    }

    static void validate(i32 year, i32 month, i32 day) {
        validateYear(year);
        if (month < 1 || month > 12) ValueError("Invalid month");
        if (day < 1 || day > daysInMonth(year, month)) ValueError("Invalid day");
    }

    /**
     * @brief 判断闰年
     */
    static bool isLeapYear(i32 year) noexcept {
        return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    }

    /**
     * @brief 返回月份天数
     */
    static i32 daysInMonth(i32 year, i32 month) noexcept {
        static constexpr i32 days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        if (month == 2 && isLeapYear(year)) return 29;
        return days[month - 1];
    }
};

const Date Date::MIN{-999999999, 1, 1};
const Date Date::MAX{999999999, 12, 31};
const Date Date::EPOCH{1970, 1, 1};

/**
 * @brief 时间
 */
class Time : public Object<Time> {
    using self = Time;

public:
    static const Time MIN;      // 00:00:00.000
    static const Time MAX;      // 23:59:59.999999999
    static const Time MIDNIGHT; // 00:00
    static const Time NOON;     // 12:00

    static constexpr i32 HOURS_PER_DAY = 24;                                       // Hours per day.
    static constexpr i32 MINUTES_PER_HOUR = 60;                                    // Minutes per hour.
    static constexpr i32 MINUTES_PER_DAY = MINUTES_PER_HOUR * HOURS_PER_DAY;       // Minutes per day.
    static constexpr i32 SECONDS_PER_MINUTE = 60;                                  // Seconds per minute.
    static constexpr i32 SECONDS_PER_HOUR = SECONDS_PER_MINUTE * MINUTES_PER_HOUR; // Seconds per hour.
    static constexpr i32 SECONDS_PER_DAY = SECONDS_PER_HOUR * HOURS_PER_DAY;       // Seconds per day.
    static constexpr i64 MILLIS_PER_DAY = SECONDS_PER_DAY * 1000LL;                // Milliseconds per day.
    static constexpr i64 MICROS_PER_DAY = SECONDS_PER_DAY * 1000'000LL;            // Microseconds per day.
    static constexpr i64 NANOS_PER_MILLI = 1000'000LL;                             // Nanos per millisecond.
    static constexpr i64 NANOS_PER_SECOND = 1000'000'000LL;                        // Nanos per second.
    static constexpr i64 NANOS_PER_MINUTE = NANOS_PER_SECOND * SECONDS_PER_MINUTE; // Nanos per minute.
    static constexpr i64 NANOS_PER_HOUR = NANOS_PER_MINUTE * MINUTES_PER_HOUR;     // Nanos per hour.
    static constexpr i64 NANOS_PER_DAY = NANOS_PER_HOUR * HOURS_PER_DAY;           // Nanos per day.

    static self of(i32 hour, i32 minute = 0, i32 second = 0, i32 nanoOfSecond = 0) {
        validate(hour, minute, second, nanoOfSecond);
        return self{hour, minute, second, nanoOfSecond};
    }

    /**
     * @brief 从当日秒数创建
     */
    static self ofSecondOfDay(i64 secondOfDay) {
        validateSecondOfDay(secondOfDay);
        i32 hour = secondOfDay / SECONDS_PER_HOUR;
        secondOfDay -= hour * SECONDS_PER_HOUR;
        i32 minute = secondOfDay / SECONDS_PER_MINUTE;
        secondOfDay -= minute * SECONDS_PER_MINUTE;
        return self{hour, minute, i32(secondOfDay)};
    }

    /**
     * @brief 从当日纳秒数创建
     */
    static self ofNanoOfDay(i64 nanoOfDay) {
        validateNanoOfDay(nanoOfDay);
        i32 hour = nanoOfDay / NANOS_PER_HOUR;
        nanoOfDay -= hour * NANOS_PER_HOUR;
        i32 minute = nanoOfDay / NANOS_PER_MINUTE;
        nanoOfDay -= minute * NANOS_PER_MINUTE;
        i32 second = nanoOfDay / NANOS_PER_SECOND;
        nanoOfDay -= second * NANOS_PER_SECOND;
        return self{hour, minute, second, i32(nanoOfDay)};
    }

    /**
     * @brief 解析"hh:mm:ss"格式
     */
    static self parse(const CString& str) {
        i32 hour, minute, second;
        if (sscanf(str.data(), "%d:%d:%d", &hour, &minute, &second) != 3) {
            ValueError("Invalid time format");
        }
        return self::of(hour, minute, second);
    }

    /**
     * @brief 获取当前系统时间，线程不安全
     */
    static self now() {
        auto now = std::chrono::system_clock::now();
        time_t time = std::chrono::system_clock::to_time_t(now);
        auto time_tm = localtime(&time);

        // 计算纳秒部分（当前秒内的时间差）
        auto diff = now - std::chrono::system_clock::from_time_t(time);
        auto nanoseconds = duration_cast<std::chrono::nanoseconds>(diff);
        return self{time_tm->tm_hour, time_tm->tm_min, time_tm->tm_sec, i32(nanoseconds.count())};
    }

    i32 hour() const { return hour_; }
    i32 minute() const { return minute_; }
    i32 second() const { return second_; }
    i32 nano() const { return nano_; }

    self plusHours(i64 hours) const {
        if (hours == 0) {
            return *this;
        }
        i32 newHour = (hours % HOURS_PER_DAY + hour_ + HOURS_PER_DAY) % HOURS_PER_DAY;
        return self{newHour, minute_, second_, nano_};
    }

    self plusMinutes(i64 minutes) const {
        if (minutes == 0) {
            return *this;
        }
        i32 mofd = hour_ * MINUTES_PER_HOUR + minute_;
        i32 newMofd = (minutes % MINUTES_PER_DAY + mofd + MINUTES_PER_DAY) % MINUTES_PER_DAY;
        if (mofd == newMofd) {
            return *this;
        }
        i32 newHour = newMofd / MINUTES_PER_HOUR;
        i32 newMinute = newMofd % MINUTES_PER_HOUR;
        return self{newHour, newMinute, second_, nano_};
    }

    self plusSeconds(i64 seconds) const {
        if (seconds == 0) {
            return *this;
        }
        i32 sofd = hour_ * SECONDS_PER_HOUR + minute_ * SECONDS_PER_MINUTE + second_;
        i32 newSofd = (seconds % SECONDS_PER_DAY + sofd + SECONDS_PER_DAY) % SECONDS_PER_DAY;
        if (sofd == newSofd) {
            return *this;
        }
        i32 newHour = newSofd / SECONDS_PER_HOUR;
        i32 newMinute = (newSofd / SECONDS_PER_MINUTE) % MINUTES_PER_HOUR;
        i32 newSecond = newSofd % SECONDS_PER_MINUTE;
        return self{newHour, newMinute, newSecond, nano_};
    }

    self plusNanos(i64 nanos) const {
        if (nanos == 0) {
            return *this;
        }
        i64 nofd = toNanoOfDay();
        i64 newNofd = ((nanos % NANOS_PER_DAY) + nofd + NANOS_PER_DAY) % NANOS_PER_DAY;
        if (nofd == newNofd) {
            return *this;
        }
        i32 newHour = newNofd / NANOS_PER_HOUR;
        i32 newMinute = (newNofd / NANOS_PER_MINUTE) % MINUTES_PER_HOUR;
        i32 newSecond = (newNofd / NANOS_PER_SECOND) % SECONDS_PER_MINUTE;
        i32 newNano = newNofd % NANOS_PER_SECOND;
        return self{newHour, newMinute, newSecond, newNano};
    }

    self minusHours(i64 hours) const {
        return plusHours(-(hours % HOURS_PER_DAY));
    }

    self minusMinutes(i64 minutes) const {
        return plusMinutes(-(minutes % MINUTES_PER_DAY));
    }

    self minusSeconds(i64 seconds) const {
        return plusSeconds(-(seconds % SECONDS_PER_DAY));
    }

    self minusNanos(i64 nanos) const {
        return plusNanos(-(nanos % NANOS_PER_DAY));
    }

    Duration operator-(const self& other) const {
        return Duration::ofSeconds(this->toSecondOfDay() - other.toSecondOfDay(), this->nano() - other.nano());
    }

    /**
     * @brief 当日总秒数
     */
    i64 toSecondOfDay() const {
        return hour_ * SECONDS_PER_HOUR + minute_ * SECONDS_PER_MINUTE + second_;
    }

    /**
     * @brief 当日总纳秒数
     */
    i64 toNanoOfDay() const {
        return hour_ * NANOS_PER_HOUR + minute_ * NANOS_PER_MINUTE + second_ * NANOS_PER_SECOND + nano_;
    }

    cmp_t __cmp__(const self& other) const {
        return this->hour_ != other.hour_     ? this->hour_ - other.hour_ :
               this->minute_ != other.minute_ ? this->minute_ - other.minute_ :
               this->second_ != other.second_ ? this->second_ - other.second_ :
                                                this->nano_ - other.nano_;
    }

    /**
     * @brief hh:mm:ss.nnnnnnnnn 格式
     */
    CString __str__() const {
        char buf[20];
        snprintf(buf, sizeof(buf), "%02d:%02d:%02d.%09d", hour_, minute_, second_, nano_);
        return CString{buf};
    }

private:
    i32 hour_;   // 0-23
    i32 minute_; // 0-59
    i32 second_; // 0-59
    i32 nano_;   // 0-999,999,999

    Time(i32 hour, i32 minute = 0, i32 second = 0, i32 nanoOfSecond = 0) :
            hour_(hour), minute_(minute), second_(second), nano_(nanoOfSecond) {}

    static void validate(i32 hour, i32 minute, i32 second, i32 nanoOfSecond) {
        if (hour < 0 || hour >= 24) ValueError("Hour out of range");
        if (minute < 0 || minute >= 60) ValueError("Minute out of range");
        if (second < 0 || second >= 60) ValueError("Second out of range");
        if (nanoOfSecond < 0 || nanoOfSecond >= 999'999'999) ValueError("Nano out of range");
    }

    static void validateSecondOfDay(i64 secondOfDay) {
        if (secondOfDay < 0 || secondOfDay >= 86400) {
            ValueError("Second of day out of range");
        }
    }

    static void validateNanoOfDay(i64 nanoOfDay) {
        if (nanoOfDay < 0 || nanoOfDay >= 86400 * 1000'000'000LL) {
            ValueError("Nano of day out of range");
        }
    }
};

const Time Time::MIN{0, 0, 0, 0};
const Time Time::MAX{23, 59, 59, 999'999'999};
const Time Time::MIDNIGHT(0, 0);
const Time Time::NOON{12, 0};

/**
 * @brief 日期时间
 */
class DateTime : public Object<DateTime> {
    using self = DateTime;

public:
    static self of(i32 year, i32 month, i32 dayOfMonth, i32 hour, i32 minute = 0, i32 second = 0, i32 nanoOfSecond = 0) {
        Date date = Date::of(year, month, dayOfMonth);
        Time time = Time::of(hour, minute, second, nanoOfSecond);
        return self{date, time};
    }

    static self of(const Date& date, const Time& time) {
        return self{date, time};
    }

    /**
     * @brief 根据纪元秒构建，TODO考虑时差
     */
    static self ofEpochSecond(i64 epochSecond, i32 nanoOfSecond);

    /**
     * @brief 解析"yyyy-MM-dd hh:mm:ss"格式
     */
    static self parse(const CString& str);

    Date toDate() const;
    Time toTime() const;

    i32 dayOfMonth() const;
    i32 dayOfYear() const;
    i32 dayOfWeek() const;

    i32 year() const;
    i32 month() const;
    i32 day() const;
    i32 hour() const;
    i32 minute() const;
    i32 second() const;
    i32 nano() const;

    /**
     * @brief 日期时间调整
     */
    self withYear(i32 year);
    self withMonth(i32 month);
    self withDayOfMonth(i32 dayOfMonth);
    self withDayOfYear(i32 dayOfYear);
    self withHour(i32 hour);
    self withMinute(i32 minute);
    self withSecond(i32 second);
    self withNano(i32 nano);

    /**
     * @brief 日期时间运算
     */
    self plusYears(i64 years);
    self plusMonths(i64 months);
    self plusWeeks(i64 weeks);
    self plusDays(i64 days);
    self plusHours(i64 hours);
    self plusMinutes(i64 minutes);
    self plusSeconds(i64 seconds);
    self plusNanos(i64 nanos);

    self minusYears(i64 years);
    self minusMonths(i64 months);
    self minusWeeks(i64 weeks);
    self minusDays(i64 days);
    self minusHours(i64 hours);
    self minusMinutes(i64 minutes);
    self minusSeconds(i64 seconds);
    self minusNanos(i64 nanos);

    cmp_t __cmp__(const self& other) const;

    CString __str__() const;

private:
    Date date_;
    Time time_;

    DateTime(const Date& date, const Time& time) :
            date_(date), time_(time) {}
};

} // namespace my::util

#endif // DATE_TIME_HPP