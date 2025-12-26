// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <chrono>
#include <string>
#include <string_view>

namespace pointless::core::DateUtils {

std::string dateStr(std::chrono::system_clock::time_point date);
std::string prettyDate(std::chrono::system_clock::time_point date, bool includeTime = false);
bool isWorkDay(std::chrono::system_clock::time_point date);
bool isToday(std::chrono::system_clock::time_point date);
bool isTomorrow(std::chrono::system_clock::time_point date);
bool isYesterday(std::chrono::system_clock::time_point date);
bool isThisWeek(std::chrono::system_clock::time_point date);
bool isNext7Days(std::chrono::system_clock::time_point date);
bool isAfterToday(std::chrono::system_clock::time_point date);
std::chrono::system_clock::time_point today();
std::chrono::system_clock::time_point trimTime(std::chrono::system_clock::time_point date);
std::chrono::system_clock::time_point thisWeeksMonday(std::chrono::system_clock::time_point date);
bool iMidnight(std::chrono::system_clock::time_point date);
std::chrono::system_clock::time_point nextMonday(std::chrono::system_clock::time_point date);
std::string_view weekdayName(std::chrono::system_clock::time_point date);

} // namespace pointless::core::DateUtils
