// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QModelIndex>
#include <QObject>
#include <QVariant>
#include <QtGlobal> // Q_EMIT

#include "weekdaymodel.h"
#include "taskfiltermodel.h"
#include "core/logger.h"
#include "date_utils.h"
#include "Clock.h"

WeekdayModel::WeekdayModel(QObject *parent)
    : QAbstractListModel(parent)
{
    _mondayDate = Gui::DateUtils::firstMondayOfWeek(Gui::Clock::today());

    for (size_t i = 0; i < _taskModels.size(); ++i) {
        auto *filter = new TaskFilterModel(this);
        _taskModels.at(i) = filter;
        _taskModels.at(i)->setDateFilter(_mondayDate.addDays(qint64(i)));
    }

    connect(this, &QAbstractListModel::rowsInserted, this, &WeekdayModel::countChanged);
    connect(this, &QAbstractListModel::rowsRemoved, this, &WeekdayModel::countChanged);
    connect(this, &QAbstractListModel::modelReset, this, &WeekdayModel::countChanged);
    connect(this, &QAbstractListModel::layoutChanged, this, &WeekdayModel::countChanged);
}

QDate WeekdayModel::mondayDate() const
{
    return _mondayDate;
}

void WeekdayModel::setMondayDate(QDate date)
{
    if (_mondayDate == date) {
        return;
    }
    beginResetModel();
    _mondayDate = date;
    for (size_t i = 0; i < _taskModels.size(); ++i) {
        _taskModels.at(i)->setDateFilter(_mondayDate.addDays(qint64(i)));
    }
    endResetModel();
    Q_EMIT mondayDateChanged();
}

WeekdayModel::~WeekdayModel() = default;

int WeekdayModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    if (!_mondayDate.isValid()) {
        P_LOG_CRITICAL("Monday date is not valid");
        return 0;
    }

    return static_cast<int>(_taskModels.size());
}

int WeekdayModel::count() const
{
    return rowCount();
}

QVariant WeekdayModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(_taskModels.size())) {
        return {};
    }

    if (role == PrettyDateRole) {
        return _mondayDate.addDays(index.row()).toString(QStringLiteral("dddd, d")).toUpper();
    }
    if (role == TasksRole) {
        return QVariant::fromValue(_taskModels.at(index.row()));
    }
    if (role == IsPastRole) {
        QDate date = _mondayDate.addDays(index.row());
        return date < Gui::Clock::today();
    }
    if (role == DateRole) {
        return _mondayDate.addDays(index.row());
    }
    return {};
}

QHash<int, QByteArray> WeekdayModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[PrettyDateRole] = "prettyDate";
    roles[TasksRole] = "tasks";
    roles[IsPastRole] = "isPast";
    roles[DateRole] = "date";
    return roles;
}
