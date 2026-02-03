// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "linux_calendar_provider.h"

namespace pointless::core {

std::vector<Calendar> LinuxCalendarProvider::getCalendars() const
{
    return {};
}

std::vector<CalendarEvent> LinuxCalendarProvider::getEvents(
    const DateRange & /*range*/,
    const std::vector<std::string> & /*calendarIds*/) const
{
    return {};
}

std::unique_ptr<CalendarProvider> createCalendarProvider()
{
    return std::make_unique<LinuxCalendarProvider>();
}

} // namespace pointless::core
