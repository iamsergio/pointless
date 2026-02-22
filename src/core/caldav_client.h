// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include "calendar_provider.h"

#include <expected>
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

    [[nodiscard]] std::expected<std::string, std::string> discoverCalendarHomeSet() const;
    [[nodiscard]] std::vector<Calendar> fetchCalendars(const std::string &homeSetUrl) const;
    [[nodiscard]] std::vector<CalendarEvent> fetchEvents(
        const std::string &calendarUrl,
        const std::string &calendarId,
        const std::string &calendarName,
        const DateRange &range) const;

    [[nodiscard]] static std::string resolveUrl(const std::string &baseUrl, const std::string &href);

private:
    [[nodiscard]] std::string discoverPrincipal() const;
    [[nodiscard]] std::string performRequest(const std::string &method, const std::string &url, const std::string &body, int depth) const;
    CalDavConfig m_config;
};

} // namespace pointless::core
