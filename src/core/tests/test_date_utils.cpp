// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "date_utils.h"
#include "Clock.h"

#include <gtest/gtest.h>

#include <chrono>
#include <ctime>

using namespace pointless::core::DateUtils;
TEST(DateUtilsTest, ThisWeeksMonday)
{
    auto now = pointless::core::Clock::now();
    auto monday = thisWeeksMonday(now);
    std::tm tm = {};
    std::time_t t = std::chrono::system_clock::to_time_t(monday);
    localtime_r(&t, &tm);
    EXPECT_EQ(tm.tm_wday, 1); // Monday

    auto next_monday = nextMonday(monday);
    std::time_t t2 = std::chrono::system_clock::to_time_t(next_monday);
    std::tm tm2 = {};
    localtime_r(&t2, &tm2);
    EXPECT_EQ(tm2.tm_wday, 1); // Monday

    auto this_sunday = monday + std::chrono::hours(24 * 6);
    std::time_t t3 = std::chrono::system_clock::to_time_t(this_sunday);
    std::tm tm3 = {};
    localtime_r(&t3, &tm3);
    EXPECT_EQ(tm3.tm_wday, 0); // Sunday
}

TEST(DateUtilsTest, IsThisWeek)
{
    auto now = pointless::core::Clock::now();
    auto monday = thisWeeksMonday(now);
    auto next_monday = nextMonday(monday);
    auto this_sunday = monday + std::chrono::hours(24 * 6);
    EXPECT_TRUE(isThisWeek(monday));
    EXPECT_FALSE(isThisWeek(next_monday));
    EXPECT_TRUE(isThisWeek(this_sunday));
}

TEST(DateUtilsTest, IsNext7Days)
{
    auto now = pointless::core::Clock::now();
    auto yesterday = now - std::chrono::hours(24);
    auto tomorrow = now + std::chrono::hours(24);
    EXPECT_TRUE(isNext7Days(now));
    EXPECT_FALSE(isNext7Days(yesterday));
    EXPECT_TRUE(isNext7Days(tomorrow));
    auto plus7 = now + std::chrono::hours(24 * 7);
    EXPECT_FALSE(isNext7Days(plus7));
}

TEST(DateUtilsTest, NextMonday)
{
    std::tm tm = {};
    tm.tm_year = 2024 - 1900;
    tm.tm_mon = 0; // January
    tm.tm_mday = 1; // Monday
    std::time_t t = std::mktime(&tm);
    auto some_monday = std::chrono::system_clock::from_time_t(t);
    tm.tm_mday = 2; // Tuesday
    t = std::mktime(&tm);
    auto some_tuesday = std::chrono::system_clock::from_time_t(t);
    tm.tm_mday = 8; // Next Monday
    t = std::mktime(&tm);
    auto next_monday = std::chrono::system_clock::from_time_t(t);
    EXPECT_EQ(weekdayName(some_monday), "Monday");
    EXPECT_EQ(weekdayName(some_tuesday), "Tuesday");
    EXPECT_EQ(weekdayName(next_monday), "Monday");
    EXPECT_EQ(weekdayName(nextMonday(some_monday)), "Monday");
    EXPECT_EQ(weekdayName(nextMonday(some_tuesday)), "Monday");
    EXPECT_EQ(nextMonday(some_monday), next_monday);
    EXPECT_EQ(nextMonday(some_tuesday), next_monday);
}

TEST(DateUtilsTest, DateStrFormat)
{
    std::tm tm = {};
    tm.tm_year = 2025 - 1900;
    tm.tm_mon = 9; // October (0-based)
    tm.tm_mday = 4;
    std::time_t t = std::mktime(&tm);
    auto tp = std::chrono::system_clock::from_time_t(t);
    EXPECT_EQ(dateStr(tp), "4-10-2025");
}

TEST(DateUtilsTest, WeekdayName)
{
    std::tm tm = {};
    tm.tm_year = 2025 - 1900;
    tm.tm_mon = 9;
    tm.tm_mday = 6; // Monday
    std::time_t t = std::mktime(&tm);
    auto tp = std::chrono::system_clock::from_time_t(t);
    EXPECT_EQ(weekdayName(tp), "Monday");
}

TEST(DateUtilsTest, IsWorkDay)
{
    std::tm tm = {};
    tm.tm_year = 2025 - 1900;
    tm.tm_mon = 9;
    tm.tm_mday = 6; // Monday
    std::time_t t = std::mktime(&tm);
    auto tp = std::chrono::system_clock::from_time_t(t);
    EXPECT_TRUE(isWorkDay(tp));
    tm.tm_mday = 5; // Sunday
    t = std::mktime(&tm);
    tp = std::chrono::system_clock::from_time_t(t);
    EXPECT_FALSE(isWorkDay(tp));
}

TEST(DateUtilsTest, TrimTime)
{
    auto now = pointless::core::Clock::now();
    auto trimmed = trimTime(now);
    std::tm tm = {};
    std::time_t t = std::chrono::system_clock::to_time_t(trimmed);
    localtime_r(&t, &tm);
    EXPECT_EQ(tm.tm_hour, 0);
    EXPECT_EQ(tm.tm_min, 0);
    EXPECT_EQ(tm.tm_sec, 0);
}

TEST(DateUtilsTest, TodayIsToday)
{
    auto t = today();
    EXPECT_TRUE(isToday(t));
}
