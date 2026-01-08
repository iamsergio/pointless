// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "calendarmodel.h"
#include "date_utils.h"
#include "Clock.h"

#include <QByteArray>
#include <QDate>
#include <QHash>
#include <QModelIndex>
#include <QObject>
#include <QVariant>

CalendarModel::CalendarModel(QObject *parent)
    : QAbstractListModel(parent)
    , _month(Gui::Clock::today())
{
}

int CalendarModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return 42;
}

QVariant CalendarModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= 42) {
        return {};
    }

    const QDate date = startDate().addDays(index.row());

    switch (static_cast<Roles>(role)) {
    case DateRole:
        return date;
    case DayRole:
        return date.day();
    case IsCurrentMonthRole:
        return date.month() == _month.month() && date.year() == _month.year();
    case IsTodayRole:
        return date == Gui::Clock::today();
    }

    return {};
}

QHash<int, QByteArray> CalendarModel::roleNames() const
{
    return {
        { DateRole, "date" },
        { DayRole, "day" },
        { IsCurrentMonthRole, "isCurrentMonth" },
        { IsTodayRole, "isToday" }
    };
}

QDate CalendarModel::month() const
{
    return _month;
}

void CalendarModel::setMonth(QDate month)
{
    if (_month == month)
        return;

    _month = month;
    Q_EMIT monthChanged();
    Q_EMIT dataChanged(index(0), index(41));
}

int CalendarModel::count() const
{
    return 42;
}

QDate CalendarModel::startDate() const
{
    const QDate firstOfMonth(_month.year(), _month.month(), 1);
    return Gui::DateUtils::firstMondayOfWeek(firstOfMonth);
}
