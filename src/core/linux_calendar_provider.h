// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include "calendar_provider.h"

namespace pointless::core {

class LinuxCalendarProvider : public CalendarProvider
{
public:
    [[nodiscard]] std::vector<Calendar> getCalendars() const override;
    [[nodiscard]] std::vector<CalendarEvent> getEvents(
        const DateRange &range,
        const std::vector<std::string> &calendarIds) const override;
};

} // namespace pointless::core
