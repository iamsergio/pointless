// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "weekdaymodel.h"


WeekdayModel::WeekdayModel(QObject *parent)
    : QAbstractListModel(parent)
{
}
QDate WeekdayModel::mondayDate() const
{
    return _mondayDate;
}

void WeekdayModel::setMondayDate(const QDate &date)
{
    if (_mondayDate == date)
        return;
    beginResetModel();
    _mondayDate = date;
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
    return 7;
}

QVariant WeekdayModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= 7)
        return QVariant();

    if (role == PrettyDateRole) {
        return _mondayDate.addDays(index.row()).toString("dddd, d");
    }
    return QVariant();
}

QHash<int, QByteArray> WeekdayModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[PrettyDateRole] = "prettyDate";
    return roles;
}
