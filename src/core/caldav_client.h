// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include "calendar_provider.h"

#include <string>
#include <vector>

namespace pointless::core {

struct CalDavConfig
{
    std::string serverUrl;
    std::string username;
    std::string password;
};

class CalDavClient
{
public:
    explicit CalDavClient(CalDavConfig config);

    [[nodiscard]] std::string discoverCalendarHomeSet() const;
    [[nodiscard]] std::vector<Calendar> fetchCalendars(const std::string &homeSetUrl) const;
    [[nodiscard]] std::vector<CalendarEvent> fetchEvents(
        const std::string &calendarUrl,
        const std::string &calendarId,
        const std::string &calendarName,
        const DateRange &range) const;

private:
    [[nodiscard]] std::string propfind(const std::string &url, const std::string &body, int depth) const;
    [[nodiscard]] std::string report(const std::string &url, const std::string &body) const;
    CalDavConfig m_config;
};

} // namespace pointless::core
