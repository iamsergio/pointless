// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <QAbstractListModel>
#include <QDate>
#include <QtQml/qqmlregistration.h>

class CalendarModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QDate month READ month WRITE setMonth NOTIFY monthChanged)
    Q_PROPERTY(int count READ count CONSTANT)

public:
    enum Roles {
        DateRole = Qt::UserRole + 1,
        DayRole,
        IsCurrentMonthRole,
        IsTodayRole
    };

    explicit CalendarModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    QDate month() const;
    void setMonth(const QDate &month);

    QDate startDate() const;

    int count() const;

Q_SIGNALS:
    void monthChanged();

private:
    QDate _month;
};
