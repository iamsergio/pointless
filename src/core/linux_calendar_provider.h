// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include "calendar_provider.h"

#include <memory>
#include <string>
#include <vector>

namespace pointless::core {

class CalDavClient;

class LinuxCalendarProvider : public CalendarProvider
{
public:
    explicit LinuxCalendarProvider(std::vector<CalDavAccountConfig> accounts = {},
                                   std::vector<ICalUrlConfig> icalUrls = {});
    ~LinuxCalendarProvider() override;
    LinuxCalendarProvider(const LinuxCalendarProvider &) = delete;
    LinuxCalendarProvider &operator=(const LinuxCalendarProvider &) = delete;
    LinuxCalendarProvider(LinuxCalendarProvider &&) noexcept = delete;
    LinuxCalendarProvider &operator=(LinuxCalendarProvider &&) noexcept = delete;

    [[nodiscard]] bool isConfigured() const override;
    [[nodiscard]] std::vector<Calendar> getCalendars() const override;
    [[nodiscard]] std::vector<CalendarEvent> getEvents(
        const DateRange &range,
        const std::vector<std::string> &calendarIds) const override;

private:
    struct CalDavAccount
    {
        std::string name;
        std::unique_ptr<CalDavClient> client;
        std::string homeSetUrl;
    };
    std::vector<CalDavAccount> m_accounts;

    struct ICalSource
    {
        std::string name;
        std::string url;
    };
    std::vector<ICalSource> m_icalSources;
};

} // namespace pointless::core
