// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "apple_calendar_provider.h"
#include <iostream>

int main()
{
    pointless::core::AppleCalendarProvider provider;
    auto calendars = provider.getCalendars();

    std::cout << "Found " << calendars.size() << " calendars:\n";
    for (const auto &cal : calendars) {
        std::cout << "Title: " << cal.title << "\n";
        std::cout << "ID: " << cal.id << "\n";
        std::cout << "Color: " << cal.color << "\n";
        std::cout << "Writable: " << (cal.writeable ? "Yes" : "No") << "\n";
        std::cout << "-------------------\n";
    }

    return 0;
}
