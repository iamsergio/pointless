// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "date_utils.h"
#include "Clock.h"

#include <array>
#include <chrono>
#include <cstdio>
#include <ctime>
#include <sstream>
#include <format>

namespace pointless::core::DateUtils {

namespace {
// Helper to convert time_point to std::tm
std::tm to_tm(std::chrono::system_clock::time_point tp)
{
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    std::tm tm {};
    ( void )localtime_r(&t, &tm);
    return tm;
}
}

std::string dateStr(std::chrono::system_clock::time_point date)
{
    std::tm tm = to_tm(date);
    std::ostringstream oss;
    oss << tm.tm_mday << "-" << (tm.tm_mon + 1) << "-" << (tm.tm_year + 1900);
    return oss.str();
}
std::string_view weekdayName(std::chrono::system_clock::time_point date)
{
    static const std::array<const char *, 7> names = { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };
    std::tm tm = to_tm(date);

    const int wday = tm.tm_wday == 0 ? 6 : tm.tm_wday - 1; // tm_wday: 0=Sunday
    return names.at(wday);
}

bool isWorkDay(std::chrono::system_clock::time_point date)
{
    std::tm tm = to_tm(date);
    return tm.tm_wday >= 1 && tm.tm_wday <= 5;
}

std::chrono::system_clock::time_point trimTime(std::chrono::system_clock::time_point date)
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

bool isToday(std::chrono::system_clock::time_point date)
{
    return trimTime(date) == today();
}

bool isTomorrow(std::chrono::system_clock::time_point date)
{
    auto yest = date - std::chrono::hours(24);
    return isToday(yest);
}

bool isYesterday(std::chrono::system_clock::time_point date)
{
    auto tom = date + std::chrono::hours(24);
    return isToday(tom);
}

std::chrono::system_clock::time_point thisWeeksMonday(std::chrono::system_clock::time_point date)
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

bool isThisWeek(std::chrono::system_clock::time_point date)
{
    auto monday = thisWeeksMonday(Clock::now());
    return date >= monday && date < monday + std::chrono::hours(24 * 7);
}

bool isNext7Days(std::chrono::system_clock::time_point date)
{
    auto t = today();
    return (isToday(date) || isAfterToday(date)) && date < t + std::chrono::hours(24 * 7);
}

bool isAfterToday(std::chrono::system_clock::time_point date)
{
    return trimTime(date) > today();
}

bool iMidnight(std::chrono::system_clock::time_point date)
{
    std::tm tm = to_tm(date);
    return tm.tm_hour == 0 && tm.tm_min == 0 && tm.tm_sec == 0;
}

std::chrono::system_clock::time_point nextMonday(std::chrono::system_clock::time_point date)
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

std::string prettyDate(std::chrono::system_clock::time_point date, bool includeTime)
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
    }

    std::string result = dateStr(date);
    if (includeTime) {
        std::tm tm = to_tm(date);
        result += " ";
        result += std::format("{:02d}:{:02d}", tm.tm_hour, tm.tm_min);
    }
    return result;
}

} // namespace pointless::core::DateUtils
