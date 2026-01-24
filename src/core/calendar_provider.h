// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <string>
#include <vector>

namespace pointless::core {

struct Calendar
{
    std::string id;
    std::string title;
    std::string color; // Hex code without alpha
    bool writeable = false;
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

    [[nodiscard]] virtual std::vector<Calendar> getCalendars() const = 0;
};

} // namespace pointless::core
