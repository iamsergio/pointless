// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "weekdaymodel.h"
#include "taskfiltermodel.h"

WeekdayModel::WeekdayModel(QObject *parent)
    : QAbstractListModel(parent)
{
    for (int i = 0; i < _taskModels.size(); ++i) {
        auto *filter = new TaskFilterModel(this);
        _taskModels[i] = filter;
    }
}
QDate WeekdayModel::mondayDate() const
{
    return _mondayDate;
}

void WeekdayModel::setMondayDate(QDate date)
{
    if (_mondayDate == date)
        return;
    beginResetModel();
    _mondayDate = date;
    for (int i = 0; i < _taskModels.size(); ++i) {
        _taskModels[i]->setDateFilter(_mondayDate.addDays(i));
    }
    endResetModel();
    Q_EMIT mondayDateChanged();
}

WeekdayModel::~WeekdayModel()
{
}

int WeekdayModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() || !_mondayDate.isValid())
        return 0;
    return static_cast<int>(_taskModels.size());
}

QVariant WeekdayModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(_taskModels.size()))
        return QVariant();

    if (role == PrettyDateRole) {
        return _mondayDate.addDays(index.row()).toString("dddd, d");
    } else if (role == TasksRole) {
        return QVariant::fromValue(_taskModels[index.row()]);
    }
    return QVariant();
}

QHash<int, QByteArray> WeekdayModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[PrettyDateRole] = "prettyDate";
    roles[TasksRole] = "tasks";
    return roles;
}
