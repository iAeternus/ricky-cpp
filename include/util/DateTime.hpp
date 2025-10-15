/**
 * @brief 日期时间，描述时间点/时间刻
 * @note 此文件参考java，故使用java命名风格
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
 * @class Date
 * @brief 日期类，描述日期相关功能
 * @details 支持日期的创建、调整、运算、转换等操作
 */
class Date : public Object<Date> {
public:
    using Self = Date;
    static const Self MIN;   // 最小可能日期（-999999999-01-01）
    static const Self MAX;   // 最大可能日期（999999999-12-31）
    static const Self EPOCH; // 基准日期（1970-01-01）

    /**
     * @brief 创建指定年、月、日的日期对象
     * @param year 年份
     * @param month 月份（1-12，默认为1月）
     * @param dayOfMonth 日期（1-31，默认为1日）
     * @return 有效的日期对象
     * @example
     * Date::of(2025, 2, 14) 创建日期 2025-02-14
     */
    static Self of(const i32 year, const i32 month = 1, const i32 dayOfMonth = 1) {
        validate(year, month, dayOfMonth);
        return Self{year, month, dayOfMonth};
    }

    /**
     * @brief 根据年份和一年中的第几天创建日期对象
     * @param year 年份
     * @param dayOfYear 一年中的第几天（1-365/366）
     * @return 有效的日期对象
     * @exception Exception 若年天数越界，则抛出 runtime_exception
     * @example
     * Date::ofYearDay(2025, 5) 创建日期 2025-01-05
     */
    static Self ofYearDay(const i32 year, const i32 dayOfYear) {
        validateYear(year);
        const bool isLeap = isLeapYear(year);
        if (dayOfYear < 1 || dayOfYear > (isLeap ? 366 : 365)) {
            throw runtime_exception("Day of year out of range");
        }

        static constexpr i32 leap[] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335};
        static constexpr i32 common[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
        const auto& offsets = isLeap ? leap : common;

        const i32 month = static_cast<i32>(std::ranges::lower_bound(offsets, dayOfYear) - offsets) - 1;
        const i32 day = dayOfYear - offsets[month];
        return Self{year, month + 1, day};
    }

    /**
     * @brief 根据纪元日（自 1970-01-01 的天数）创建日期对象
     * @param epochDay 纪元日
     * @return 对应的日期对象
     */
    static Self ofEpochDay(const i64 epochDay) {
        return ofEpochDayImpl(epochDay);
    }

    /**
     * @brief 解析 "yyyy-MM-dd" 格式的日期字符串
     * @param str 日期字符串
     * @return 解析成功的日期对象
     * @exception Exception 若字符串格式不正确，抛出 runtime_exception
     * @example
     * Date::parse("2025-02-14") 解析为 2025-02-14
     */
    static Self parse(const CString& str) {
        i32 year, month, day;
        if (sscanf_s(str.data(), "%d-%d-%d", &year, &month, &day) != 3) {
            throw runtime_exception("Invalid date format");
        }
        return of(year, month, day);
    }

    /**
     * @brief 获取当前日期（基于 UTC 时间，线程不安全实现）
     * @return 当前日期对象
     * @note 该实现依赖于 `localtime` 函数，是线程不安全的 TODO
     */
    static Self now() {
        const auto now = std::chrono::system_clock::now();
        const time_t time = std::chrono::system_clock::to_time_t(now);
        const auto time_tm = std::localtime(&time);
        return Self{time_tm->tm_year + 1900, time_tm->tm_mon + 1, time_tm->tm_mday};
    }

    // 获取年份
    i32 year() const noexcept { return year_; }

    // 获取月份
    i32 month() const noexcept { return month_; }

    // 获取日期
    i32 day() const noexcept { return day_; }

    // 获取日期（同 day()）
    i32 dayOfMonth() const noexcept { return day_; }

    /**
     * @brief 获取一年中的第几天
     * @return 一年中的第几天
     */
    i32 dayOfYear() const {
        static constexpr i32 leap[] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335};
        static constexpr i32 common[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
        return (isLeapYear() ? leap : common)[month_ - 1] + day_;
    }

    /**
     * @brief 计算星期，返回值为 1-7（周一至周日）
     * @return 星期数 [1,7]
     */
    i32 dayOfWeek() const noexcept {
        const i64 epochDay = toEpochDay();
        return static_cast<i32>(epochDay >= -4 ? (epochDay + 4) % 7 : (epochDay + 5) % 7 + 6);
    }

    /**
     * @brief 判断是否为闰年
     * @return 是否为闰年
     */
    bool isLeapYear() const noexcept {
        return isLeapYear(year_);
    }

    /**
     * @brief 日期运算方法，支持加减年份
     * @param years 年份数
     * @return 新的日期对象
     */
    Self plusYears(const i64 years) const {
        return adjustDay(year_ + years, month_);
    }

    /**
     * @brief 日期运算方法，加减月份
     * @param months 月份数
     * @return 新的日期对象
     */
    Self plusMonths(const i64 months) const {
        const i64 total = year_ * 12LL + (month_ - 1) + months;
        i64 newYear = total / 12;
        i32 newMonth = total % 12 + 1;
        if (newMonth < 1) {
            newMonth += 12;
            newYear -= 1;
        }
        return adjustDay(newYear, newMonth);
    }

    /**
     * @brief 日期运算方法，加减周数
     * @param weeks 周数
     * @return 新的日期对象
     */
    Self plusWeeks(const i64 weeks) const {
        return plusDays(math::mul_exact(weeks, 7LL));
    }

    /**
     * @brief 日期运算方法，加减天数
     * @param days 天数
     * @return 新的日期对象
     */
    Self plusDays(const i64 days) const {
        return ofEpochDay(toEpochDay() + days);
    }

    /**
     * @brief 日期运算方法，减年份
     * @param years 年份数
     * @return 新的日期对象
     */
    Self minusYears(const i64 years) const {
        return plusYears(-years);
    }

    /**
     * @brief 日期运算方法，减月份数
     * @param months 月份数
     * @return 新的日期对象
     */
    Self minusMonths(const i64 months) const {
        return plusMonths(-months);
    }

    /**
     * @brief 日期运算方法，减周数
     * @param weeks 周数
     * @return 新的日期对象
     */
    Self minusWeeks(const i64 weeks) const {
        return minusDays(math::mul_exact(weeks, 7LL));
    }

    /**
     * @brief 日期运算方法，减天数
     * @param days 天数
     * @return 新的日期对象
     */
    Self minusDays(const i64 days) const {
        return plusDays(-days);
    }

    /**
     * @brief 日期调整方法，设置年份
     * @param year 新的年份
     * @return 新的日期对象，若年份不变则返回自身
     */
    Self withYear(const i32 year) const {
        if (year == year_) {
            return *this;
        }
        return adjustDay(year, month_);
    }

    /**
     * @brief 日期调整方法，设置月份
     * @param month 新的月份
     * @return 新的日期对象，若月份不变则返回自身
     */
    Self withMonth(const i32 month) const {
        if (month == month_) {
            return *this;
        }
        return adjustDay(year_, month);
    }

    /**
     * @brief 日期调整方法，设置日期
     * @param day 新的日期
     * @return 新的日期对象，若日期不变则返回自身
     */
    Self withDay(const i32 day) const {
        if (day == day_) {
            return *this;
        }
        return of(year_, month_, day);
    }

    /**
     * @brief 日期调整方法，设置日期（同 withDay）
     * @param datOfMonth 新的日期
     * @return 新的日期对象
     */
    Self withDayOfMonth(const i32 datOfMonth) const {
        return of(year_, month_, datOfMonth);
    }

    /**
     * @brief 日期调整方法，设置一年中的第几天
     * @param dayOfYear 一年中的第几天
     * @return 新的日期对象
     */
    Self withDayOfYear(const i32 dayOfYear) const {
        return ofYearDay(year_, dayOfYear);
    }

    /**
     * @brief 获取本月的最后一天
     * @return 本月最后一天的日期对象
     */
    Self lastDayOfMonth() const {
        return Self{year_, month_, daysInMonth(year_, month_)};
    }

    /**
     * @brief 两个日期对象的差值，返回 Duration 类型对象
     * @param other 另一个日期对象
     * @return 日期差值（以天为单位）
     */
    Duration operator-(const Self& other) const {
        return Duration::ofDays(toEpochDay() - other.toEpochDay());
    }

    /**
     * @brief 转换为纪元日，第0天为 1970-01-01
     * @return 纪元日（i64 型）
     */
    i64 toEpochDay() const {
        const i32 y = year_ - (month_ <= 2);
        const i32 m = month_;
        const i32 d = day_;
        const i64 era = (y >= 0 ? y : y - 399) / 400;
        const u32 yoe = y - era * 400;
        const u32 doy = (153 * (m > 2 ? m - 3 : m + 9) + 2) / 5 + d - 1;
        const u32 doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
        return era * 146097 + doe - 719468;
    }

    /**
     * @brief 比较两个日期对象
     * @param other 另一个日期对象
     * @return 比较结果（-1、0、1），0 表示相等
     */
    [[nodiscard]] cmp_t __cmp__(const Self& other) const {
        return this->year_ != other.year_   ? this->year_ - other.year_ :
               this->month_ != other.month_ ? this->month_ - other.month_ :
                                              this->day_ - other.day_;
    }

    /**
     * @brief 返回日期的字符串表示（YYYY-MM-DD 格式）
     * @return 日期字符串
     */
    [[nodiscard]] CString __str__() const {
        char buf[11];
        snprintf(buf, sizeof(buf), "%04d-%02d-%02d", year_, month_, day_);
        return CString{buf};
    }

private:
    /**
     * @brief 私有构造函数，确保只能通过静态方法创建对象
     * @param year 年份
     * @param month 月份
     * @param day 日期
     */
    Date(const i32 year, const i32 month, const i32 day) :
            year_(year), month_(month), day_(day) {}

    static Self ofEpochDayImpl(const i64 epochDay) {
        const i64 z = epochDay + 719468;
        const i64 era = (z >= 0 ? z : z - 146096) / 146097;
        const u32 doe = static_cast<u32>(z - era * 146097);
        const u32 yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
        i64 y = yoe + era * 400;
        const u32 doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
        const u32 mp = (5 * doy + 2) / 153;
        const u32 d = doy - (153 * mp + 2) / 5 + 1;
        const u32 m = mp < 10 ? mp + 3 : mp - 9;
        y += (m <= 2);
        return Self{static_cast<i32>(y), static_cast<i32>(m), static_cast<i32>(d)};
    }

    /**
     * @brief 调整日期到有效范围
     * @param newYear 新的年份
     * @param newMonth 新的月份
     * @return 调整后的日期对象
     */
    Self adjustDay(i64 newYear, i32 newMonth) const {
        newYear = clampYear(newYear);
        newMonth = clampMonth(newMonth);
        const i32 newDay = math::min_(day_, daysInMonth(newYear, newMonth));
        return Self{static_cast<i32>(newYear), newMonth, newDay};
    }

    /**
     * @brief 检查年份是否在有效范围内
     * @param year 年份
     * @return 正常年份
     * @exception Exception 若年份越界，则抛出 runtime_exception
     */
    static i64 clampYear(const i64 year) {
        if (year < -999999999 || year > 999999999) {
            throw runtime_exception("Year overflow");
        }
        return year;
    }

    /**
     * @brief 检查月份是否在有效范围内
     * @param month 月份
     * @return 正常月份
     * @exception Exception 若月份非法，则抛出 runtime_exception
     */
    static i32 clampMonth(const i32 month) {
        if (month < 1 || month > 12) {
            throw runtime_exception("Invalid month");
        }
        return month;
    }

    /**
     * @brief 校验年份是否有效
     * @param year 年份
     * @exception Exception 若年份越界，则抛出 runtime_exception
     */
    static void validateYear(const i32 year) {
        if (year < -999999999 || year > 999999999) throw runtime_exception("year out of range");
    }

    /**
     * @brief 校验年、月、日是否有效
     * @param year 年份
     * @param month 月份
     * @param day 日期
     * @exception Exception 若月份非法，则抛出 runtime_exception
     * @exception Exception 若月天数非法，则抛出 runtime_exception
     */
    static void validate(const i32 year, const i32 month, const i32 day) {
        validateYear(year);
        if (month < 1 || month > 12) throw runtime_exception("invalid month");
        if (day < 1 || day > daysInMonth(year, month)) throw runtime_exception("invalid day");
    }

    /**
     * @brief 判断是否为闰年
     * @param year 年份
     * @return 是否为闰年
     */
    static bool isLeapYear(const i32 year) noexcept {
        return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    }

    /**
     * @brief 获取指定年份和月份的天数
     * @param year 年份
     * @param month 月份
     * @return 该月的天数
     */
    static i32 daysInMonth(const i32 year, const i32 month) noexcept {
        static constexpr i32 days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        if (month == 2 && isLeapYear(year)) return 29;
        return days[month - 1];
    }

private:
    i32 year_;  // 年份，范围 [-999999999, 999999999]
    i32 month_; // 月份，范围 [1,12]
    i32 day_;   // 日期，范围 [1,31]
};

inline const Date Date::MIN{-999999999, 1, 1};
inline const Date Date::MAX{999999999, 12, 31};
inline const Date Date::EPOCH{1970, 1, 1};

/**
 * @class Time
 * @brief 时间类，描述时间相关功能
 * @details 支持时间的创建、调整、运算、转换等操作
 */
class Time : public Object<Time> {
public:
    using Self = Time;
    static const Self MIN;      // 最小可能时间（00:00:00.000）
    static const Self MAX;      // 最大可能时间（23:59:59.999999999）
    static const Self MIDNIGHT; // 午夜（00:00）
    static const Self NOON;     // 中午（12:00）

    static constexpr i32 HOURS_PER_DAY = 24;                                       // 每日小时数
    static constexpr i32 MINUTES_PER_HOUR = 60;                                    // 每小时分钟数
    static constexpr i32 MINUTES_PER_DAY = MINUTES_PER_HOUR * HOURS_PER_DAY;       // 每日分钟数
    static constexpr i32 SECONDS_PER_MINUTE = 60;                                  // 每分钟秒数
    static constexpr i32 SECONDS_PER_HOUR = SECONDS_PER_MINUTE * MINUTES_PER_HOUR; // 每小时秒数
    static constexpr i32 SECONDS_PER_DAY = SECONDS_PER_HOUR * HOURS_PER_DAY;       // 每日秒数
    static constexpr i64 MILLIS_PER_DAY = SECONDS_PER_DAY * 1000LL;                // 每日毫秒数
    static constexpr i64 MICROS_PER_DAY = SECONDS_PER_DAY * 1000'000LL;            // 每日微秒数
    static constexpr i64 NANOS_PER_MILLI = 1000'000LL;                             // 每毫秒纳秒数
    static constexpr i64 NANOS_PER_SECOND = 1000'000'000LL;                        // 每秒纳秒数
    static constexpr i64 NANOS_PER_MINUTE = NANOS_PER_SECOND * SECONDS_PER_MINUTE; // 每分钟纳秒数
    static constexpr i64 NANOS_PER_HOUR = NANOS_PER_MINUTE * MINUTES_PER_HOUR;     // 每小时纳秒数
    static constexpr i64 NANOS_PER_DAY = NANOS_PER_HOUR * HOURS_PER_DAY;           // 每日纳秒数

    /**
     * @brief 创建指定时、分、秒、纳秒的时间对象
     * @param hour 小时（0-23）
     * @param minute 分钟（0-59）
     * @param second 秒（0-59）
     * @param nanoOfSecond 纳秒（0-999999999）
     * @return 时间对象
     * @example
     * Time::of(12, 30, 45) 创建时间 12:30:45
     */
    static Self of(const i32 hour, const i32 minute = 0, const i32 second = 0, const i32 nanoOfSecond = 0) {
        validate(hour, minute, second, nanoOfSecond);
        return Self{hour, minute, second, nanoOfSecond};
    }

    /**
     * @brief 根据当日秒数创建时间对象
     * @param secondOfDay 当日秒数（0-86399）
     * @return 时间对象
     * @exception throw runtime_exception 若秒数超出范围
     */
    static Self ofSecondOfDay(i64 secondOfDay) {
        validateSecondOfDay(secondOfDay);
        const i32 hour = secondOfDay / SECONDS_PER_HOUR;
        secondOfDay -= hour * SECONDS_PER_HOUR;
        const i32 minute = secondOfDay / SECONDS_PER_MINUTE;
        secondOfDay -= minute * SECONDS_PER_MINUTE;
        return Self{hour, minute, static_cast<i32>(secondOfDay)};
    }

    /**
     * @brief 根据当日纳秒数创建时间对象
     * @param nanoOfDay 当日纳秒数（0-86399999999999）
     * @return 时间对象
     * @exception throw runtime_exception 若纳秒数超出范围
     */
    static Self ofNanoOfDay(i64 nanoOfDay) {
        validateNanoOfDay(nanoOfDay);
        const i32 hour = nanoOfDay / NANOS_PER_HOUR;
        nanoOfDay -= hour * NANOS_PER_HOUR;
        const i32 minute = nanoOfDay / NANOS_PER_MINUTE;
        nanoOfDay -= minute * NANOS_PER_MINUTE;
        const i32 second = nanoOfDay / NANOS_PER_SECOND;
        nanoOfDay -= second * NANOS_PER_SECOND;
        return Self{hour, minute, second, static_cast<i32>(nanoOfDay)};
    }

    /**
     * @brief 解析 "hh:mm:ss" 格式的时间字符串
     * @param str 时间字符串
     * @return 解析成功的时间对象
     * @exception throw runtime_exception 格式无效
     * @example
     * Time::parse("12:30:45") 解析为 12:30:45
     */
    static Self parse(const CString& str) {
        i32 hour, minute, second;
        if (sscanf(str.data(), "%d:%d:%d", &hour, &minute, &second) != 3) {
            throw runtime_exception("invalid time format");
        }
        return of(hour, minute, second);
    }

    /**
     * @brief 获取当前系统时间（线程不安全）
     * @return 当前时间对象
     * @note 该实现依赖于 `localtime` 函数，是线程不安全的 TODO
     */
    static Self now() {
        const auto now = std::chrono::system_clock::now();
        const time_t time = std::chrono::system_clock::to_time_t(now);
        const auto time_tm = std::localtime(&time);

        // 计算纳秒部分（当前秒内的时间差）
        const auto diff = now - std::chrono::system_clock::from_time_t(time);
        const auto nanoseconds = duration_cast<std::chrono::nanoseconds>(diff);
        return Self{time_tm->tm_hour, time_tm->tm_min, time_tm->tm_sec, static_cast<i32>(nanoseconds.count())};
    }

    // 获取小时
    i32 hour() const { return hour_; }

    // 获取分钟
    i32 minute() const { return minute_; }

    // 获取秒
    i32 second() const { return second_; }

    // 获取纳秒
    i32 nano() const { return nano_; }

    /**
     * @brief 时间运算方法，加小时
     * @param hours 小时数
     * @return 新的时间对象
     */
    Self plusHours(const i64 hours) const {
        if (hours == 0) {
            return *this;
        }
        const i32 newHour = (hours % HOURS_PER_DAY + hour_ + HOURS_PER_DAY) % HOURS_PER_DAY;
        return Self{newHour, minute_, second_, nano_};
    }

    /**
     * @brief 时间运算方法，加分
     * @param minutes 分钟数
     * @return 新的时间对象
     */
    Self plusMinutes(const i64 minutes) const {
        if (minutes == 0) {
            return *this;
        }
        const i32 mofd = hour_ * MINUTES_PER_HOUR + minute_;
        const i32 newMofd = (minutes % MINUTES_PER_DAY + mofd + MINUTES_PER_DAY) % MINUTES_PER_DAY;
        if (mofd == newMofd) {
            return *this;
        }
        const i32 newHour = newMofd / MINUTES_PER_HOUR;
        const i32 newMinute = newMofd % MINUTES_PER_HOUR;
        return Self{newHour, newMinute, second_, nano_};
    }

    /**
     * @brief 时间运算方法，加秒
     * @param seconds 秒数
     * @return 新的时间对象
     */
    Self plusSeconds(const i64 seconds) const {
        if (seconds == 0) {
            return *this;
        }
        const i32 sofd = hour_ * SECONDS_PER_HOUR + minute_ * SECONDS_PER_MINUTE + second_;
        const i32 newSofd = (seconds % SECONDS_PER_DAY + sofd + SECONDS_PER_DAY) % SECONDS_PER_DAY;
        if (sofd == newSofd) {
            return *this;
        }
        const i32 newHour = newSofd / SECONDS_PER_HOUR;
        const i32 newMinute = (newSofd / SECONDS_PER_MINUTE) % MINUTES_PER_HOUR;
        const i32 newSecond = newSofd % SECONDS_PER_MINUTE;
        return Self{newHour, newMinute, newSecond, nano_};
    }

    /**
     * @brief 时间运算方法，加纳秒
     * @param nanos 纳秒数
     * @return 新的时间对象
     */
    Self plusNanos(const i64 nanos) const {
        if (nanos == 0) {
            return *this;
        }
        const i64 nofd = toNanoOfDay();
        const i64 newNofd = ((nanos % NANOS_PER_DAY) + nofd + NANOS_PER_DAY) % NANOS_PER_DAY;
        if (nofd == newNofd) {
            return *this;
        }
        const i32 newHour = newNofd / NANOS_PER_HOUR;
        const i32 newMinute = (newNofd / NANOS_PER_MINUTE) % MINUTES_PER_HOUR;
        const i32 newSecond = (newNofd / NANOS_PER_SECOND) % SECONDS_PER_MINUTE;
        const i32 newNano = newNofd % NANOS_PER_SECOND;
        return Self{newHour, newMinute, newSecond, newNano};
    }

    /**
     * @brief 时间运算方法，减小时
     * @param hours 小时数
     * @return 新的时间对象
     */
    Self minusHours(const i64 hours) const {
        return plusHours(-(hours % HOURS_PER_DAY));
    }

    /**
     * @brief 时间运算方法，减分
     * @param minutes 分钟数
     * @return 新的时间对象
     */
    Self minusMinutes(const i64 minutes) const {
        return plusMinutes(-(minutes % MINUTES_PER_DAY));
    }

    /**
     * @brief 时间运算方法，减秒
     * @param seconds 秒数
     * @return 新的时间对象
     */
    Self minusSeconds(const i64 seconds) const {
        return plusSeconds(-(seconds % SECONDS_PER_DAY));
    }

    /**
     * @brief 时间运算方法，减纳秒
     * @param nanos 纳秒数
     * @return 新的时间对象
     */
    Self minusNanos(const i64 nanos) const {
        return plusNanos(-(nanos % NANOS_PER_DAY));
    }

    /**
     * @brief 计算两个时间点的差值，返回 Duration 类型对象
     * @param other 另一个时间点
     * @return 时间差值（秒和纳秒）
     */
    Duration operator-(const Self& other) const {
        return Duration::ofSeconds(this->toSecondOfDay() - other.toSecondOfDay(), this->nano() - other.nano());
    }

    /**
     * @brief 设置小时
     * @param hour 小时
     * @return 新的时间对象，若小时不变则返回自身
     */
    Self withHour(const i32 hour) const {
        if (hour == hour_) {
            return *this;
        }
        return Self{hour, minute_, second_, nano_};
    }

    /**
     * @brief 设置分钟
     * @param minute 分钟
     * @return 新的时间对象，若分钟不变则返回自身
     */
    Self withMinute(const i32 minute) const {
        if (minute == minute_) {
            return *this;
        }
        return Self{hour_, minute, second_, nano_};
    }

    /**
     * @brief 设置秒
     * @param second 秒
     * @return 新的时间对象，若秒不变则返回自身
     */
    Self withSecond(const i32 second) const {
        if (second == second_) {
            return *this;
        }
        return Self{hour_, minute_, second, nano_};
    }

    /**
     * @brief 设置纳秒
     * @param nanoOfSecond 纳秒
     * @return 新的时间对象，若纳秒不变则返回自身
     */
    Self withNano(const i32 nanoOfSecond) const {
        if (nanoOfSecond == nano_) {
            return *this;
        }
        return Self{hour_, minute_, second_, nanoOfSecond};
    }

    /**
     * @brief 获取当日经过的总秒数
     * @return 当日秒数
     */
    i64 toSecondOfDay() const {
        return hour_ * SECONDS_PER_HOUR + minute_ * SECONDS_PER_MINUTE + second_;
    }

    /**
     * @brief 获取当日经过的总纳秒数
     * @return 当日纳秒数
     */
    i64 toNanoOfDay() const {
        return hour_ * NANOS_PER_HOUR + minute_ * NANOS_PER_MINUTE + second_ * NANOS_PER_SECOND + nano_;
    }

    /**
     * @brief 比较两个时间点
     * @param other 另一个时间点
     * @return 比较结果（-1、0、1），0 表示相等
     */
    [[nodiscard]] cmp_t __cmp__(const Self& other) const {
        return this->hour_ != other.hour_     ? this->hour_ - other.hour_ :
               this->minute_ != other.minute_ ? this->minute_ - other.minute_ :
               this->second_ != other.second_ ? this->second_ - other.second_ :
                                                this->nano_ - other.nano_;
    }

    /**
     * @brief 返回时间的字符串表示（HH:MM:SS.nnnnnnnnn 格式）
     * @return 时间字符串
     */
    [[nodiscard]] CString __str__() const {
        char buf[20];
        snprintf(buf, sizeof(buf), "%02d:%02d:%02d.%09d", hour_, minute_, second_, nano_);
        return CString{buf};
    }

private:
    /**
     * @brief 私有构造函数，确保只能通过静态方法创建对象
     * @param hour 小时
     * @param minute 分钟
     * @param second 秒
     * @param nanoOfSecond 纳秒
     */
    explicit Time(const i32 hour, const i32 minute = 0, const i32 second = 0, const i32 nanoOfSecond = 0) :
            hour_(hour), minute_(minute), second_(second), nano_(nanoOfSecond) {}

    /**
     * @brief 校验时间参数是否有效
     * @param hour 小时
     * @param minute 分钟
     * @param second 秒
     * @param nanoOfSecond 纳秒
     * @exception Exception 若小时越界，则抛出 runtime_exception
     * @exception Exception 若分钟数越界，则抛出 runtime_exception
     * @exception Exception 若秒数越界，则抛出 runtime_exception
     * @exception Exception 若纳秒数越界，则抛出 runtime_exception
     */
    static void validate(const i32 hour, const i32 minute, const i32 second, const i32 nanoOfSecond) {
        if (hour < 0 || hour >= 24) throw runtime_exception("hour out of range");
        if (minute < 0 || minute >= 60) throw runtime_exception("minute out of range");
        if (second < 0 || second >= 60) throw runtime_exception("second out of range");
        if (nanoOfSecond < 0 || nanoOfSecond >= 999'999'999) throw runtime_exception("nano out of range");
    }

    /**
     * @brief 校验当日秒数是否有效
     * @param secondOfDay 当日秒数
     * @exception Exception 若当日秒数越界，则抛出 runtime_exception
     */
    static void validateSecondOfDay(const i64 secondOfDay) {
        if (secondOfDay < 0 || secondOfDay >= 86400) {
            throw runtime_exception("second of day out of range");
        }
    }

    /**
     * @brief 校验当日纳秒数是否有效
     * @param nanoOfDay 当日纳秒数
     * @exception Exception 若当日纳秒数越界，则抛出 runtime_exception
     */
    static void validateNanoOfDay(const i64 nanoOfDay) {
        if (nanoOfDay < 0 || nanoOfDay >= 86400 * 1000'000'000LL) {
            throw runtime_exception("nano of day out of range");
        }
    }

private:
    i32 hour_;   // 小时，范围 0-23
    i32 minute_; // 分钟，范围 0-59
    i32 second_; // 秒，范围 0-59
    i32 nano_;   // 纳秒，范围 0-999999999
};

inline const Time Time::MIN{0, 0, 0, 0};
inline const Time Time::MAX{23, 59, 59, 999'999'999};
inline const Time Time::MIDNIGHT(0, 0);
inline const Time Time::NOON{12, 0};

/**
 * @class DateTime
 * @brief 日期时间类，描述日期和时间的组合
 * @details 支持日期时间的创建、调整、运算、转换等操作
 */
class DateTime : public Object<DateTime> {
public:
    using Self = DateTime;

    /**
     * @brief 创建指定日期和时间的日期时间对象
     * @param year 年份
     * @param month 月份
     * @param dayOfMonth 日期
     * @param hour 小时
     * @param minute 分钟
     * @param second 秒
     * @param nanoOfSecond 纳秒
     * @return 日期时间对象
     * @example
     * DateTime::of(2025, 2, 14, 12, 30) 创建日期时间 2025-02-14 12:30:00
     */
    static Self of(const i32 year,
                   const i32 month,
                   const i32 dayOfMonth,
                   const i32 hour,
                   const i32 minute = 0,
                   const i32 second = 0,
                   const i32 nanoOfSecond = 0) {
        const Date date = Date::of(year, month, dayOfMonth);
        const Time time = Time::of(hour, minute, second, nanoOfSecond);
        return Self{date, time};
    }

    /**
     * @brief 创建指定日期和时间的日期时间对象
     * @param date 日期对象
     * @param time 时间对象
     * @return 日期时间对象
     */
    static Self of(const Date& date, const Time& time) {
        return Self{date, time};
    }

    /**
     * @brief 根据纪元秒构建日期时间对象（不考虑时差）
     * @param epochSecond 纪元秒
     * @param nanoOfSecond 纳秒（可选）
     * @return 日期时间对象
     * @exception Exception 若纳秒数越界，则抛出 runtime_exception
     * TODO 考虑时差的影响
     */
    static Self ofEpochSecond(const i64 epochSecond, const i32 nanoOfSecond = 0) {
        if (nanoOfSecond < 0 || nanoOfSecond >= 999'999'999) throw runtime_exception("nano out of range");
        const i32 epochDay = epochSecond / Time::SECONDS_PER_DAY;
        const i32 secsOfDay = epochSecond % Time::SECONDS_PER_DAY;
        const auto date = Date::ofEpochDay(epochDay);
        const auto time = Time::ofNanoOfDay(secsOfDay * Time::NANOS_PER_SECOND + nanoOfSecond);
        return Self{date, time};
    }

    /**
     * @brief 解析 "yyyy-MM-dd hh:mm:ss" 格式的日期时间字符串
     * @param str 日期时间字符串
     * @return 解析成功的时间对象
     * @exception Exception 若格式无效，则抛出 runtime_exception
     * @example
     * DateTime::parse("2025-02-14 12:30:45") 解析为 2025-02-14 12:30:45
     */
    static Self parse(const CString& str) {
        i32 year, month, day, hour, minute, second;
        if (sscanf(str.data(), "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second) != 6) {
            throw runtime_exception("invalid date time format");
        }
        return Self::of(year, month, day, hour, minute, second);
    }

    /**
     * @brief 获取当前日期时间（基于 UTC 时间）
     * @return 当前日期时间对象
     * @note 该实现依赖于 `localtime` 函数，是线程不安全的 TODO
     */
    static Self now() {
        const auto now = std::chrono::system_clock::now();
        const time_t time = std::chrono::system_clock::to_time_t(now);
        const auto time_tm = std::localtime(&time);
        const auto diff = now - std::chrono::system_clock::from_time_t(time);
        const auto nanoseconds = duration_cast<std::chrono::nanoseconds>(diff);
        return Self{Date::of(time_tm->tm_year + 1900, time_tm->tm_mon + 1, time_tm->tm_mday),
                    Time::of(time_tm->tm_hour, time_tm->tm_min, time_tm->tm_sec, static_cast<i32>(nanoseconds.count()))};
    }

    // 获取日期部分
    Date toDate() const { return date_; }

    // 获取时间部分
    Time toTime() const { return time_; }

    /**
     * @brief 获取日期中的日期部分
     * @return 日期
     */
    i32 dayOfMonth() const { return date_.dayOfMonth(); }

    /**
     * @brief 获取日期中的一年中的第几天
     * @return 一年中的第几天
     */
    i32 dayOfYear() const { return date_.dayOfYear(); }

    /**
     * @brief 获取星期（1-7，周一至周日）
     * @return 星期数
     */
    i32 dayOfWeek() const { return date_.dayOfWeek(); }

    // 获取年份
    i32 year() const { return date_.year(); }

    // 获取月份
    i32 month() const { return date_.month(); }

    // 获取日期
    i32 day() const { return date_.day(); }

    // 获取小时
    i32 hour() const { return time_.hour(); }

    // 获取分钟
    i32 minute() const { return time_.minute(); }

    // 获取秒
    i32 second() const { return time_.second(); }

    // 获取纳秒
    i32 nano() const { return time_.nano(); }

    /**
     * @brief 调整日期和时间
     * @param newDate 新的日期
     * @param newTime 新的时间
     * @return 新的日期时间对象
     */
    Self with(const Date& newDate, const Time& newTime) const {
        if (date_ == newDate && time_ == newTime) {
            return *this;
        }
        return Self{newDate, newTime};
    }

    /**
     * @brief 设置年份
     * @param year 新的年份
     * @return 新的日期时间对象
     */
    Self withYear(const i32 year) const {
        return with(date_.withYear(year), time_);
    }

    /**
     * @brief 设置月份
     * @param month 新的月份
     * @return 新的日期时间对象
     */
    Self withMonth(const i32 month) const {
        return with(date_.withMonth(month), time_);
    }

    /**
     * @brief 设置日期（同 dayOfMonth）
     * @param dayOfMonth 新的日期
     * @return 新的日期时间对象
     */
    Self withDayOfMonth(const i32 dayOfMonth) const {
        return with(date_.withDayOfMonth(dayOfMonth), time_);
    }

    /**
     * @brief 设置一年中的第几天
     * @param dayOfYear 一年中的第几天
     * @return 新的日期时间对象
     */
    Self withDayOfYear(const i32 dayOfYear) const {
        return with(date_.withDayOfYear(dayOfYear), time_);
    }

    /**
     * @brief 设置小时
     * @param hour 新的小时
     * @return 新的日期时间对象
     */
    Self withHour(const i32 hour) const {
        return with(date_, time_.withHour(hour));
    }

    /**
     * @brief 设置分钟
     * @param minute 新的分钟
     * @return 新的日期时间对象
     */
    Self withMinute(const i32 minute) const {
        return with(date_, time_.withMinute(minute));
    }

    /**
     * @brief 设置秒
     * @param second 新的秒
     * @return 新的日期时间对象
     */
    Self withSecond(const i32 second) const {
        return with(date_, time_.withSecond(second));
    }

    /**
     * @brief 设置纳秒
     * @param nano 新的纳秒
     * @return 新的日期时间对象
     */
    Self withNano(const i32 nano) const {
        return with(date_, time_.withNano(nano));
    }

    /**
     * @brief 日期时间运算方法，加年份
     * @param years 年份数
     * @return 新的日期时间对象
     */
    Self plusYears(const i64 years) const {
        return with(date_.plusYears(years), time_);
    }

    /**
     * @brief 日期时间运算方法，加月份数
     * @param months 月份数
     * @return 新的日期时间对象
     */
    Self plusMonths(const i64 months) const {
        return with(date_.plusMonths(months), time_);
    }

    /**
     * @brief 日期时间运算方法，加周数
     * @param weeks 周数
     * @return 新的日期时间对象
     */
    Self plusWeeks(const i64 weeks) const {
        return with(date_.plusWeeks(weeks), time_);
    }

    /**
     * @brief 日期时间运算方法，加天数
     * @param days 天数
     * @return 新的日期时间对象
     */
    Self plusDays(const i64 days) const {
        return with(date_.plusDays(days), time_);
    }

    /**
     * @brief 日期时间运算方法，加小时
     * @param hours 小时数
     * @return 新的日期时间对象
     */
    Self plusHours(const i64 hours) const {
        return with(date_, time_.plusHours(hours));
    }

    /**
     * @brief 日期时间运算方法，加分
     * @param minutes 分钟数
     * @return 新的日期时间对象
     */
    Self plusMinutes(const i64 minutes) const {
        return with(date_, time_.plusMinutes(minutes));
    }

    /**
     * @brief 日期时间运算方法，加秒
     * @param seconds 秒数
     * @return 新的日期时间对象
     */
    Self plusSeconds(const i64 seconds) const {
        return with(date_, time_.plusSeconds(seconds));
    }

    /**
     * @brief 日期时间运算方法，加纳秒
     * @param nanos 纳秒数
     * @return 新的日期时间对象
     */
    Self plusNanos(const i64 nanos) const {
        return with(date_, time_.plusNanos(nanos));
    }

    /**
     * @brief 日期时间运算方法，减年份
     * @param years 年份数
     * @return 新的日期时间对象
     */
    Self minusYears(const i64 years) const {
        return with(date_.minusYears(years), time_);
    }

    /**
     * @brief 日期时间运算方法，减月份数
     * @param months 月份数
     * @return 新的日期时间对象
     */
    Self minusMonths(const i64 months) const {
        return with(date_.minusMonths(months), time_);
    }

    /**
     * @brief 日期时间运算方法，减周数
     * @param weeks 周数
     * @return 新的日期时间对象
     */
    Self minusWeeks(const i64 weeks) const {
        return with(date_.minusWeeks(weeks), time_);
    }

    /**
     * @brief 日期时间运算方法，减天数
     * @param days 天数
     * @return 新的日期时间对象
     */
    Self minusDays(const i64 days) const {
        return with(date_.minusDays(days), time_);
    }

    /**
     * @brief 日期时间运算方法，减小时
     * @param hours 小时数
     * @return 新的日期时间对象
     */
    Self minusHours(const i64 hours) const {
        return with(date_, time_.minusHours(hours));
    }

    /**
     * @brief 日期时间运算方法，减分
     * @param minutes 分钟数
     * @return 新的日期时间对象
     */
    Self minusMinutes(const i64 minutes) const {
        return with(date_, time_.minusMinutes(minutes));
    }

    /**
     * @brief 日期时间运算方法，减秒
     * @param seconds 秒数
     * @return 新的日期时间对象
     */
    Self minusSeconds(const i64 seconds) const {
        return with(date_, time_.minusSeconds(seconds));
    }

    /**
     * @brief 日期时间运算方法，减纳秒
     * @param nanos 纳秒数
     * @return 新的日期时间对象
     */
    Self minusNanos(const i64 nanos) const {
        return with(date_, time_.minusNanos(nanos));
    }

    /**
     * @brief 计算两个日期时间之间的差值，返回 Duration 类型对象
     * @param other 另一个日期时间对象
     * @return 日期时间差值
     */
    Duration operator-(const Self& other) const {
        return this->toDuration() - other.toDuration();
    }

    /**
     * @brief 转换为纪元秒（Epoch Seconds）
     * @return 纪元秒
     */
    i64 toEpochSecond() const {
        const i64 epochDay = date_.toEpochDay();
        const i64 secondOfDay = time_.toSecondOfDay();
        return epochDay * Time::SECONDS_PER_DAY + secondOfDay;
    }

    /**
     * @brief 转换为 Duration 类型对象
     * @return Duration 对象
     */
    Duration toDuration() const {
        const i64 epochDay = date_.toEpochDay();
        const i64 secondOfDay = time_.toSecondOfDay();
        const i32 nanoOfSecond = time_.toNanoOfDay() - secondOfDay * Time::NANOS_PER_SECOND;
        return Duration::ofSeconds(epochDay * Time::SECONDS_PER_DAY + secondOfDay, nanoOfSecond);
    }

    /**
     * @brief 比较两个日期时间
     * @param other 另一个日期时间对象
     * @return 比较结果（-1、0、1），0 表示相等
     */
    [[nodiscard]] cmp_t __cmp__(const Self& other) const {
        const auto date_cmp = date_.__cmp__(other.date_);
        return date_cmp == 0 ? time_.__cmp__(other.time_) : date_cmp;
    }

    /**
     * @brief 返回日期时间的字符串表示（如：2025-02-14T12:30:45.000000000Z）
     * @return 日期时间字符串
     */
    [[nodiscard]] CString __str__() const {
        std::stringstream stream;
        stream << date_.__str__().data() << 'T' << time_.__str__().data() << 'Z';
        return CString{stream.str()};
    }

private:
    /**
     * @brief 私有构造函数，确保只能通过静态方法创建对象
     * @param date 日期对象
     * @param time 时间对象
     */
    DateTime(const Date& date, const Time& time) :
            date_(date), time_(time) {}

private:
    Date date_; // 日期部分
    Time time_; // 时间部分
};

} // namespace my::util

#endif // DATE_TIME_HPP