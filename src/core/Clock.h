// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <chrono>

namespace PointlessCore {

class Clock
{
public:
    static std::chrono::system_clock::time_point now();

#ifdef POINTLESS_DEVELOPER_MODE
    static void setTestNow(const std::chrono::system_clock::time_point &t);
    static void reset();

private:
    static std::optional<std::chrono::system_clock::time_point> s_testTime;
#endif
};

} // namespace PointlessCore
