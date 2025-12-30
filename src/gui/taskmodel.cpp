// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "taskmodel.h"
#include "data_controller.h"
#include "gui_controller.h"

#include "core/logger.h"

#include <QDateTime>
#include <QString>

TaskModel::TaskModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

TaskModel *TaskModel::instance(QObject *parent)
{
    if (_instance == nullptr) {
        _instance = new TaskModel(parent);
    }
    return _instance;
}

int TaskModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    const auto &data = localData();
    return static_cast<int>(data.taskCount());
}

QVariant TaskModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= static_cast<int>(localData().taskCount())) {
        return {};
    }

    const auto &task = localData().data()._data.tasks[index.row()];

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
            QString dateStr = dt.toString(QStringLiteral("MMM d"));
            if (dt.time() != QTime(0, 0)) {
                dateStr += QStringLiteral(", ") + dt.toString(QStringLiteral("HH:mm"));
            }
            return dateStr;
        }
        return {};
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
        return {};
    case IsEveningRole:
        if (task.dueDate) {
            const auto timeT = std::chrono::system_clock::to_time_t(*task.dueDate);
            const QDateTime dt = QDateTime::fromSecsSinceEpoch(timeT);
            return dt.time().hour() >= 17;
        }
        return false;
    default:
        return {};
    }

    return {};
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
    roles[IsEveningRole] = "isEvening";
    return roles;
}

int TaskModel::count() const
{
    return rowCount();
}

void TaskModel::reload()
{
    beginResetModel();
    P_LOG_INFO("size = {}", static_cast<int>(localData().taskCount()));
    endResetModel();
    emit countChanged();
}

void TaskModel::addTask(const pointless::core::Task &task)
{
    const int numTasks = static_cast<int>(localData().taskCount());
    beginInsertRows(QModelIndex(), numTasks, numTasks);
    localData().data().addTask(task);
    endInsertRows();
    emit countChanged();
}

const pointless::core::Task *TaskModel::taskAt(int row) const
{
    if (row < 0 || row >= static_cast<int>(localData().taskCount())) // NOLINT
        return nullptr;
    return &localData().data()._data.tasks[row];
}

const pointless::core::Task *TaskModel::taskForUuid(const QString &taskUuid) const
{
    std::string uuidStr = taskUuid.toStdString();
    const auto &tasks = localData().data()._data.tasks;
    for (const auto &task : tasks) {
        if (task.uuid == uuidStr) {
            return &task;
        }
    }
    return nullptr;
}

const pointless::core::LocalData &TaskModel::localData() const
{
    auto *guiController = GuiController::instance();
    return guiController->dataController()->localData();
}

pointless::core::LocalData &TaskModel::localData()
{
    auto *guiController = GuiController::instance();
    return guiController->dataController()->localData();
}
