// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "Clock.h"

#ifdef POINTLESS_DEVELOPER_MODE
#include "core/Clock.h"
#endif

namespace Gui {

#ifdef POINTLESS_DEVELOPER_MODE
std::optional<QDateTime> Clock::s_testTime = std::nullopt;
#endif

QDateTime Clock::now()
{
#ifdef POINTLESS_DEVELOPER_MODE
    if (s_testTime.has_value()) {
        return s_testTime.value();
    }
#endif
    return QDateTime::currentDateTime();
}

QDate Clock::today()
{
    return now().date();
}

#ifdef POINTLESS_DEVELOPER_MODE
void Clock::setTestNow(const QDateTime &t)
{
    s_testTime = t;
    auto millis = t.toMSecsSinceEpoch();
    auto tp = std::chrono::system_clock::time_point(std::chrono::milliseconds(millis));
    pointless::core::Clock::setTestNow(tp);
}

void Clock::reset()
{
    s_testTime = std::nullopt;
    pointless::core::Clock::reset();
}
#endif

} // namespace Gui
