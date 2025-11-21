// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <QDate>
#include <QDateTime>

#include <chrono>
#include <optional>
#include <cassert>

namespace Gui {
namespace DateUtils {

inline QDate timepointToQDate(const std::optional<std::chrono::system_clock::time_point> &tp)
{
    if (!tp.has_value())
        return QDate();
    std::time_t tt = std::chrono::system_clock::to_time_t(tp.value());
    return QDateTime::fromSecsSinceEpoch(static_cast<qint64>(tt)).date();
}

inline bool isMonday(const QDate &date)
{
    return date.dayOfWeek() == 1;
}

inline QDate firstMondayOfWeek(const QDate &date)
{
    if (isMonday(date))
        return date;
    return date.addDays(1 - date.dayOfWeek());
}


inline bool isOverdue(QDate dueDate, QDate currentDate)
{
    assert(currentDate.isValid());
    return dueDate.isValid() && currentDate.isValid() && dueDate < currentDate;
}

inline bool isToday(QDate date)
{
    return date == QDate::currentDate();
}

} // namespace DateUtils
} // namespace Gui
