// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include "task.h"

#include <QAbstractListModel>
#include <QtQml/qqmlregistration.h>

#include <cstdint>
#include <vector>

class TaskModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    enum Roles : std::uint16_t {
        UuidRole = Qt::UserRole + 1,
        TitleRole,
        IsDoneRole,
        IsImportantRole,
        DueDateRole,
        HasDueDateRole,
        TagNameRole,
        IsFromCalendarRole,
        CalendarNameRole,
        IsEveningRole
    };

    [[nodiscard]] static TaskModel *instance(QObject *parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    [[nodiscard]] int count() const;

    void setTasks(const std::vector<PointlessCore::Task> &tasks);
    void addTask(const PointlessCore::Task &task);
    [[nodiscard]] const PointlessCore::Task *taskAt(int row) const;
    [[nodiscard]] const PointlessCore::Task *taskForUuid(const QString &taskUuid) const;

Q_SIGNALS:
    void countChanged();

private:
    explicit TaskModel(QObject *parent = nullptr);
    inline static TaskModel *_instance = nullptr;
    std::vector<PointlessCore::Task> _tasks;
};
