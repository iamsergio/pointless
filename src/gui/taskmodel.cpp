#include "taskmodel.h"

TaskModel *TaskModel::instance(QObject *parent)
{
    if (!_instance)
        _instance = new TaskModel(parent);
    return _instance;
}
// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "taskmodel.h"
#include "logger.h"

#include <QDateTime>

TaskModel::TaskModel(QObject *parent)
    : QAbstractListModel(parent)
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
            const auto timeT = std::chrono::system_clock::to_time_t(*task.dueDate);
            const QDateTime dt = QDateTime::fromSecsSinceEpoch(timeT);
            QString dateStr = dt.toString("MMM d");
            if (dt.time() != QTime(0, 0)) {
                dateStr += ", " + dt.toString("HH:mm");
            }
            return dateStr;
        }
        return QVariant();
    case HasDueDateRole:
        return task.dueDate.has_value();
    case TagNameRole:
        return QString::fromStdString(task.tagName());
    case IsFromCalendarRole:
        return task.deviceCalendarName.has_value() && !task.deviceCalendarName->empty();
    case CalendarNameRole:
        if (task.deviceCalendarName && !task.deviceCalendarName->empty()) {
            return QString::fromStdString(*task.deviceCalendarName);
        }
    default:
        return QVariant();
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
    roles[HasDueDateRole] = "hasDueDate";
    roles[TagNameRole] = "tagName";
    roles[IsFromCalendarRole] = "isFromCalendar";
    roles[CalendarNameRole] = "calendarName";
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

const PointlessCore::Task *TaskModel::taskForUuid(const QString &taskUuid) const
{
    std::string uuidStr = taskUuid.toStdString();
    for (const auto &task : _tasks) {
        if (task.uuid == uuidStr) {
            return &task;
        }
    }
    return nullptr;
}
