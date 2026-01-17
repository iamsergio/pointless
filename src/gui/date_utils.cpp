// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "date_utils.h"
#include "Clock.h"

#include <QDate>
#include <QDateTime>

#include <cassert>

namespace Gui::DateUtils {

QDate timepointToQDate(std::optional<std::chrono::system_clock::time_point> tp)
{
    if (!tp.has_value()) {
        return {};
    }
    std::time_t tt = std::chrono::system_clock::to_time_t(tp.value());
    return QDateTime::fromSecsSinceEpoch(static_cast<qint64>(tt)).date();
}

std::optional<std::chrono::system_clock::time_point> qdateToTimepoint(QDate date)
{
    if (!date.isValid()) {
        return std::nullopt;
    }
    QDateTime dt(date, QTime(0, 0));
    return std::chrono::system_clock::from_time_t(dt.toSecsSinceEpoch());
}

bool isMonday(QDate date)
{
    return date.dayOfWeek() == 1;
}

QDate firstMondayOfWeek(QDate date)
{
    if (isMonday(date)) {
        return date;
    }
    return date.addDays(1 - date.dayOfWeek());
}

QDate nextMonday(QDate date)
{
    const int dayOfWeek = date.dayOfWeek();
    const int daysToAdd = (dayOfWeek == 1) ? 7 : (8 - dayOfWeek);
    return date.addDays(daysToAdd);
}

bool isOverdue(QDate dueDate, QDate currentDate)
{
    assert(currentDate.isValid());
    return dueDate.isValid() && currentDate.isValid() && dueDate < currentDate;
}

bool isToday(QDate date)
{
    return date == Gui::Clock::today();
}

} // namespace Gui::DateUtils
