// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <QDateTime>

#ifdef POINTLESS_DEVELOPER_MODE
#include <optional>
#endif

namespace Gui {

class Clock
{
public:
    // Returns the current time, or the fixed test time if set
    static QDateTime now();

    // Helper for just the date
    static QDate today();

#ifdef POINTLESS_DEVELOPER_MODE
    // Test helpers
    static void setTestNow(const QDateTime &t);
    static void reset(); // Go back to using system time
private:
    static std::optional<QDateTime> s_testTime;
#endif
};

} // namespace Gui
