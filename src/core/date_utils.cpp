// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "date_utils.h"
#include "Clock.h"

#include <ctime>
#include <sstream>
#include <format>

namespace PointlessCore::DateUtils {

namespace {
// Helper to convert time_point to std::tm
std::tm to_tm(const std::chrono::system_clock::time_point &tp)
{
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    std::tm tm {};
    localtime_r(&t, &tm);
    return tm;
}
}

std::string dateStr(const std::chrono::system_clock::time_point &date)
{
    std::tm tm = to_tm(date);
    std::ostringstream oss;
    oss << tm.tm_mday << "-" << (tm.tm_mon + 1) << "-" << (tm.tm_year + 1900);
    return oss.str();
}

std::string_view weekdayName(const std::chrono::system_clock::time_point &date)
{
    static const char *names[] = { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };
    std::tm tm = to_tm(date);
    int wday = tm.tm_wday == 0 ? 6 : tm.tm_wday - 1; // tm_wday: 0=Sunday
    return names[wday];
}

bool isWorkDay(const std::chrono::system_clock::time_point &date)
{
    std::tm tm = to_tm(date);
    return tm.tm_wday >= 1 && tm.tm_wday <= 5;
}

std::chrono::system_clock::time_point trimTime(const std::chrono::system_clock::time_point &date)
{
    std::tm tm = to_tm(date);
    tm.tm_hour = 0;
    tm.tm_min = 0;
    tm.tm_sec = 0;
    tm.tm_isdst = -1; // Let mktime determine DST
    std::time_t t = mktime(&tm);
    return std::chrono::system_clock::from_time_t(t);
}

std::chrono::system_clock::time_point today()
{
    return trimTime(Clock::now());
}

bool isToday(const std::chrono::system_clock::time_point &date)
{
    return trimTime(date) == today();
}

bool isTomorrow(const std::chrono::system_clock::time_point &date)
{
    auto yest = date - std::chrono::hours(24);
    return isToday(yest);
}

bool isYesterday(const std::chrono::system_clock::time_point &date)
{
    auto tom = date + std::chrono::hours(24);
    return isToday(tom);
}

std::chrono::system_clock::time_point thisWeeksMonday(const std::chrono::system_clock::time_point &date)
{
    std::tm tm = to_tm(date);
    int daysBack = (tm.tm_wday == 0 ? 6 : tm.tm_wday - 1);
    tm.tm_mday -= daysBack;
    tm.tm_hour = 0;
    tm.tm_min = 0;
    tm.tm_sec = 0;
    tm.tm_isdst = -1;
    std::time_t t = mktime(&tm);
    return std::chrono::system_clock::from_time_t(t);
}

std::chrono::system_clock::time_point thisWeeksMonday()
{
    auto monday = thisWeeksMonday(Clock::now());
    return trimTime(monday);
}

bool isThisWeek(const std::chrono::system_clock::time_point &date)
{
    auto monday = thisWeeksMonday(Clock::now());
    return date >= monday && date < monday + std::chrono::hours(24 * 7);
}

bool isNext7Days(const std::chrono::system_clock::time_point &date)
{
    auto t = today();
    return (isToday(date) || isAfterToday(date)) && date < t + std::chrono::hours(24 * 7);
}

bool isAfterToday(const std::chrono::system_clock::time_point &date)
{
    return trimTime(date) > today();
}

bool iMidnight(const std::chrono::system_clock::time_point &date)
{
    std::tm tm = to_tm(date);
    return tm.tm_hour == 0 && tm.tm_min == 0 && tm.tm_sec == 0;
}

std::chrono::system_clock::time_point nextMonday(const std::chrono::system_clock::time_point &date)
{
    std::tm tm = to_tm(date);
    int daysBack = (tm.tm_wday == 0 ? 6 : tm.tm_wday - 1);
    tm.tm_mday += (7 - daysBack);
    tm.tm_hour = 0;
    tm.tm_min = 0;
    tm.tm_sec = 0;
    tm.tm_isdst = -1;
    std::time_t t = mktime(&tm);
    return std::chrono::system_clock::from_time_t(t);
}

std::string prettyDate(const std::chrono::system_clock::time_point &date, bool includeTime)
{
    if (isToday(date)) {
        return includeTime ? std::format("today {}", weekdayName(date)) : "today";
    }
    if (isTomorrow(date)) {
        return "tomorrow";
    }
    if (isYesterday(date)) {
        return "yesterday";
    }
    if (isThisWeek(date)) {
        return std::format("this {}", weekdayName(date));
    }
    if (isNext7Days(date)) {
        return std::format("next {}", weekdayName(date));
    } else {
        std::string result = dateStr(date);
        if (includeTime) {
            std::tm tm = to_tm(date);
            char buf[6];
            snprintf(buf, sizeof(buf), "%02d:%02d", tm.tm_hour, tm.tm_min);
            result += " ";
            result += buf;
        }
        return result;
    }
}

} // namespace PointlessCore::DateUtils
