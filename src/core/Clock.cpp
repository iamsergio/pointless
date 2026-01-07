// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "Clock.h"

#include <chrono>

namespace pointless::core {

#ifdef POINTLESS_DEVELOPER_MODE
std::optional<std::chrono::system_clock::time_point> Clock::s_testTime = std::nullopt;
#endif

std::chrono::system_clock::time_point Clock::now()
{
#ifdef POINTLESS_DEVELOPER_MODE
    if (s_testTime.has_value()) {
        return s_testTime.value();
    }
#endif
    return std::chrono::system_clock::now();
}

#ifdef POINTLESS_DEVELOPER_MODE
void Clock::setTestNow(std::chrono::system_clock::time_point t)
{
    s_testTime = t;
}

void Clock::reset()
{
    s_testTime = std::nullopt;
}
#endif

} // namespace pointless::core
