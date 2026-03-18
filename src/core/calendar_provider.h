// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <vector>

namespace pointless::core {

struct Calendar
{
    std::string id;
    std::string title;
    std::string color; // Hex code without alpha
    bool writeable = false;
    std::string accountName;
};

struct DateRange
{
    std::chrono::system_clock::time_point start;
    std::chrono::system_clock::time_point end;
};

struct CalendarEvent
{
    std::string eventId;
    std::string calendarId;
    std::string calendarName;
    std::string title;
    std::chrono::system_clock::time_point startDate;
    std::chrono::system_clock::time_point endDate;
    bool isAllDay = false;
};

class CalendarProvider
{
public:
    virtual ~CalendarProvider() = default;

    CalendarProvider() = default;
    CalendarProvider(const CalendarProvider &) = delete;
    CalendarProvider(CalendarProvider &&) = delete;
    CalendarProvider &operator=(const CalendarProvider &) = delete;
    CalendarProvider &operator=(CalendarProvider &&) = delete;

    [[nodiscard]] virtual bool isConfigured() const = 0;
    [[nodiscard]] virtual std::vector<Calendar> getCalendars() const = 0;
    [[nodiscard]] virtual std::vector<CalendarEvent> getEvents(
        const DateRange &range,
        const std::vector<std::string> &calendarIds) const = 0;
};

struct CalDavAccountConfig
{
    std::string name;
    std::string url;
    std::string username;
    std::string password;
};

struct ICalUrlConfig
{
    std::string name;
    std::string url;
};

std::unique_ptr<CalendarProvider> createCalendarProvider(
    const std::string &caldavUrl = {},
    const std::string &caldavUsername = {},
    const std::string &caldavPassword = {});

std::unique_ptr<CalendarProvider> createCalendarProvider(
    std::vector<CalDavAccountConfig> accounts);

std::unique_ptr<CalendarProvider> createCalendarProvider(
    std::vector<CalDavAccountConfig> accounts,
    std::vector<ICalUrlConfig> icalUrls);

} // namespace pointless::core
