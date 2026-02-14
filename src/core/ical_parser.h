// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <chrono>
#include <string>
#include <vector>

namespace pointless::core {

struct ICalEvent
{
    std::string uid;
    std::string summary;
    std::chrono::system_clock::time_point dtstart;
    std::chrono::system_clock::time_point dtend;
    bool isAllDay = false;
};

std::vector<ICalEvent> parseICalEvents(const std::string &icalData);

} // namespace pointless::core
