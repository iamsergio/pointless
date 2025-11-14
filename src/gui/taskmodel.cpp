// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "taskmodel.h"
#include "logger.h"

#include <QDateTime>

TaskModel::TaskModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

TaskModel::TaskModel(const std::vector<PointlessCore::Task> &tasks, QObject *parent)
    : QAbstractListModel(parent)
    , _tasks(tasks)
{
}

int TaskModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return static_cast<int>(_tasks.size());
}

QVariant TaskModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= static_cast<int>(_tasks.size()))
        return QVariant();

    const auto &task = _tasks[index.row()];

    switch (role) {
    case UuidRole:
        return QString::fromStdString(task.uuid);
    case TitleRole:
        return QString::fromStdString(task.title);
    case IsDoneRole:
        return task.isDone;
    case IsImportantRole:
        return task.isImportant;
    case DueDateRole:
        if (task.dueDate) {
            auto timeT = std::chrono::system_clock::to_time_t(*task.dueDate);
            return QDateTime::fromSecsSinceEpoch(timeT);
        }
        return QVariant();
    case TagNameRole:
        return QString::fromStdString(task.tagName());
    }

    return QVariant();
}

QHash<int, QByteArray> TaskModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[UuidRole] = "uuid";
    roles[TitleRole] = "title";
    roles[IsDoneRole] = "isDone";
    roles[IsImportantRole] = "isImportant";
    roles[DueDateRole] = "dueDate";
    roles[TagNameRole] = "tagName";
    return roles;
}

int TaskModel::count() const
{
    return rowCount();
}

void TaskModel::setTasks(const std::vector<PointlessCore::Task> &tasks)
{
    beginResetModel();
    _tasks = tasks;
    P_LOG_INFO("size = {}", static_cast<int>(_tasks.size()));
    endResetModel();
    emit countChanged();
}

const PointlessCore::Task *TaskModel::taskAt(int row) const
{
    if (row < 0 || row >= static_cast<int>(_tasks.size()))
        return nullptr;
    return &_tasks[row];
}
