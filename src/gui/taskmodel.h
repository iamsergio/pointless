// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include "task.h"

#include <QAbstractListModel>
#include <QtQml/qqmlregistration.h>

#include <vector>

class TaskModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    enum Roles {
        UuidRole = Qt::UserRole + 1,
        TitleRole,
        IsDoneRole,
        IsImportantRole,
        DueDateRole,
        HasDueDateRole,
        TagNameRole,
        IsFromCalendarRole,
        CalendarNameRole
    };

    static TaskModel *instance(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    int count() const;

    void setTasks(const std::vector<PointlessCore::Task> &tasks);
    const PointlessCore::Task *taskAt(int row) const;

Q_SIGNALS:
    void countChanged();

private:
    explicit TaskModel(QObject *parent = nullptr);
    inline static TaskModel *_instance = nullptr;
    std::vector<PointlessCore::Task> _tasks;
};
