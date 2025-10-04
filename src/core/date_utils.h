// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <chrono>
#include <string>

namespace PointlessCore {
namespace DateUtils {

std::string dateStr(const std::chrono::system_clock::time_point &date);
std::string prettyDate(const std::chrono::system_clock::time_point &date, bool includeTime = false);
bool isWorkDay(const std::chrono::system_clock::time_point &date);
bool isToday(const std::chrono::system_clock::time_point &date);
bool isTomorrow(const std::chrono::system_clock::time_point &date);
bool isYesterday(const std::chrono::system_clock::time_point &date);
bool isThisWeek(const std::chrono::system_clock::time_point &date);
bool isNext7Days(const std::chrono::system_clock::time_point &date);
bool isAfterToday(const std::chrono::system_clock::time_point &date);
std::chrono::system_clock::time_point today();
std::chrono::system_clock::time_point trimTime(const std::chrono::system_clock::time_point &date);
std::chrono::system_clock::time_point thisWeeksMonday(const std::chrono::system_clock::time_point &date);
bool iMidnight(const std::chrono::system_clock::time_point &date);
std::chrono::system_clock::time_point nextMonday(const std::chrono::system_clock::time_point &date);
std::string weekdayName(const std::chrono::system_clock::time_point &date);

} // namespace DateUtils
} // namespace PointlessCore
