// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT
#pragma once

#include <QAbstractListModel>
#include <QtQml/qqmlregistration.h>
#include <QDate>

#include <array>

class TaskFilterModel;

class WeekdayModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QDate mondayDate READ mondayDate WRITE setMondayDate NOTIFY mondayDateChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    explicit WeekdayModel(QObject *parent = nullptr);
    ~WeekdayModel() override;

    enum Roles {
        PrettyDateRole = Qt::UserRole + 1,
        TasksRole,
        IsPastRole
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int count() const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    QDate mondayDate() const;
    void setMondayDate(QDate date);

Q_SIGNALS:
    void mondayDateChanged();
    void countChanged();

private:
    QDate _mondayDate;
    std::array<TaskFilterModel *, 7> _taskModels;
};
