/**
* @brief 日期时间，描述时间点/时间刻
* @author Ricky
* @date 2025/2/3
* @version 1.0
*/
#ifndef DATE_TIME_HPP
#define DATE_TIME_HPP

#include "Duration.hpp"

namespace my::util {

/**
* @brief 日期类
*/
class Date : public Object<Date> {
    using self = Date;

public:
    static self of(i32 year, i32 month = 1, i32 day = 1) {
        validate(year, month, day);
        return self{year, month, day};
    }

    i32 year() const noexcept { return year_; }
    i32 month() const noexcept { return month_; }
    i32 day() const noexcept { return day_; }

    self plusDays(i32 days) const {
        return self(year_, month_, day_ + days).normalize();
    }

    self plusMonths(i32 months) const {
        i32 totalMonths = year_ * 12 + (month_ - 1) + months;
        i32 newYear = totalMonths / 12;
        i32 newMonth = totalMonths % 12 + 1;
        return self(newYear, newMonth, day_).adjustDay().normalize();
    }

    self plusYears(i32 years) const {
        return self(year_ + years, month_, day_).adjustDay().normalize();
    }

    self minusDays(int days) const {
        return self(year_, month_, day_ - days).normalize();
    }

    self minusMonths(int months) const {
        i32 totalMonths = year_ * 12 + (month_ - 1) - months;
        i32 newYear = totalMonths / 12;
        i32 newMonth = totalMonths % 12 + 1;
        return self(newYear, newMonth, day_).adjustDay().normalize();
    }

    self minusYears(int years) const {
        return self(year_ - years, month_, day_).adjustDay().normalize();
    }

    Duration operator-(const self& other) const {
        return Duration::ofDays(toEpochDay() - other.toEpochDay());
    }

    i64 toEpochDay() const {
        i64 y = year_;
        i64 m = month_;
        i64 total = 365 * y;

        if (y >= 0) {
            total += (y + 3) / 4 - (y + 99) / 100 + (y + 399) / 400;
        } else {
            total -= (-y) / 4 - (-y) / 100 + (-y) / 400;
        }

        total += (367 * m - 362) / 12;
        total += day_ - 1;

        if (m > 2) {
            total -= isLeapYear(y) ? 1 : 2;
        }

        return total - DAYS_0000_TO_1970;
    }

    cmp_t __cmp__(const self& other) const {
        return math::subtractExact(toEpochDay(), other.toEpochDay());
    }

    CString __str__() const {
        char buf[11];
        snprintf(buf, sizeof(buf), "%04d-%02d-%02d", year_, month_, day_);
        return CString(buf);
    }

private:
    i32 year_;  // [-9999, 9999]
    i32 month_; // [1, 12]
    i32 day_;

    Date(i32 year, i32 month, i32 day) :
            year_(year), month_(month), day_(day) {}

    /**
    * @brief 从0000年到1970年的天数
    */
    static constexpr i64 DAYS_0000_TO_1970 = 719528LL;

    static void validate(i32 year, i32 month, i32 day) {
        if (year < -9999 || year > 9999) ValueError("Year out of range");
        if (month < 1 || month > 12) ValueError("Month out of range");
        if (day < 1 || day > daysInMonth(year, month)) ValueError("Day out of range");
    }

    static i32 daysInMonth(i32 year, i32 month) {
        static constexpr i32 days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        return (month == 2 && isLeapYear(year)) ? 29 : days[month - 1];
    }

    static bool isLeapYear(i32 year) {
        return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    }

    self normalize() const {
        i32 y = year_;
        i32 m = month_;
        i32 d = day_;

        while (d > daysInMonth(y, m)) {
            d -= daysInMonth(y, m);
            if (++m > 12) {
                m = 1;
                y++;
            }
        }

        while (d < 1) {
            if (--m < 1) {
                m = 12;
                y--;
            }
            d += daysInMonth(y, m);
        }

        return self(y, m, d);
    }

    self adjustDay() const {
        return self(year_, month_, std::min(day_, daysInMonth(year_, month_)));
    }
};

/**
* @brief 时间
*/
class Time : public Object<Time> {
    using self = Time;

public:
private:
    i32 hour_;
    i32 minute_;
    i32 second_;
    i32 nano_;
};

/**
* @brief 日期时间
*/
class DateTime : public Object<DateTime> {
    using self = DateTime;

public:
private:
    Date date;
    Time time;
};

} // namespace my::util

#endif // DATE_TIME_HPP