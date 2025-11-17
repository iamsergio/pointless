// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT
#pragma once

#include <QAbstractListModel>
#include <QtQml/qqmlregistration.h>
#include <QDate>

class WeekdayModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QDate mondayDate READ mondayDate WRITE setMondayDate NOTIFY mondayDateChanged)
public:
    explicit WeekdayModel(QObject *parent = nullptr);
    ~WeekdayModel() override;

    enum Roles {
        PrettyDateRole = Qt::UserRole + 1
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    QDate mondayDate() const;
    void setMondayDate(const QDate &date);

Q_SIGNALS:
    void mondayDateChanged();

private:
    QDate _mondayDate;
};
