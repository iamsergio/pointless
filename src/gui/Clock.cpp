// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "Clock.h"

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
}

void Clock::reset()
{
    s_testTime = std::nullopt;
}
#endif

} // namespace Gui
