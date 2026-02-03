// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include "calendar_provider.h"

#include <memory>

namespace pointless::core {

class AppleCalendarProvider : public CalendarProvider
{
public:
    AppleCalendarProvider();
    ~AppleCalendarProvider() override;

    AppleCalendarProvider(const AppleCalendarProvider &) = delete;
    AppleCalendarProvider(AppleCalendarProvider &&) = delete;
    AppleCalendarProvider &operator=(const AppleCalendarProvider &) = delete;
    AppleCalendarProvider &operator=(AppleCalendarProvider &&) = delete;

    [[nodiscard]] std::vector<Calendar> getCalendars() const override;
    [[nodiscard]] std::vector<CalendarEvent> getEvents(
        const DateRange &range,
        const std::vector<std::string> &calendarIds) const override;

private:
    struct Private;
    std::unique_ptr<Private> _d;
};

} // namespace pointless::core
