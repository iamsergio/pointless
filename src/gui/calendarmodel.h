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
    enum Roles : uint16_t {
        DateRole = Qt::UserRole + 1,
        DayRole,
        IsCurrentMonthRole,
        IsTodayRole
    };

    explicit CalendarModel(QObject *parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    [[nodiscard]] QDate month() const;
    void setMonth(QDate month);

    [[nodiscard]] QDate startDate() const;

    [[nodiscard]] int count() const;

Q_SIGNALS:
    void monthChanged();

private:
    QDate _month;
};
