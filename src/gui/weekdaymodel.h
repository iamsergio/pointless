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

    WeekdayModel(const WeekdayModel &) = delete;
    WeekdayModel &operator=(const WeekdayModel &) = delete;
    WeekdayModel(WeekdayModel &&) = delete;
    WeekdayModel &operator=(WeekdayModel &&) = delete;

    enum Roles : uint16_t {
        PrettyDateRole = Qt::UserRole + 1,
        TasksRole,
        IsPastRole,
        DateRole
    };

    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] int count() const;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    [[nodiscard]] QDate mondayDate() const;
    void setMondayDate(QDate date);

Q_SIGNALS:
    void mondayDateChanged();
    void countChanged();

private:
    QDate _mondayDate;
    std::array<TaskFilterModel *, 7> _taskModels = {};
};
