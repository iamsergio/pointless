// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include "calendar_provider.h"

#include <memory>
#include <string>

namespace pointless::core {

class CalDavClient;

class LinuxCalendarProvider : public CalendarProvider
{
public:
    LinuxCalendarProvider();
    ~LinuxCalendarProvider() override;
    LinuxCalendarProvider(const LinuxCalendarProvider &) = delete;
    LinuxCalendarProvider &operator=(const LinuxCalendarProvider &) = delete;
    LinuxCalendarProvider(LinuxCalendarProvider &&) noexcept;
    LinuxCalendarProvider &operator=(LinuxCalendarProvider &&) noexcept;

    [[nodiscard]] std::vector<Calendar> getCalendars() const override;
    [[nodiscard]] std::vector<CalendarEvent> getEvents(
        const DateRange &range,
        const std::vector<std::string> &calendarIds) const override;

private:
    std::unique_ptr<CalDavClient> m_client;
    std::string m_homeSetUrl;
};

} // namespace pointless::core
