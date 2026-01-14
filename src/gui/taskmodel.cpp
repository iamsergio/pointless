// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "taskmodel.h"
#include "data_controller.h"
#include "gui_controller.h"

#include "gui/qt_logger.h"

#include <QDateTime>
#include <QString>

using namespace pointless;

TaskModel::TaskModel(QObject *parent)
    : QAbstractListModel(parent)
{
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

    const auto &task = localData().taskAt(index.row());

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
        return QString();
    case HasDueDateRole:
        return task.dueDate.has_value();
    case TagNameRole:
        return QString::fromStdString(task.tagName()).toLower();
    case IsFromCalendarRole:
        return task.deviceCalendarName.has_value() && !task.deviceCalendarName->empty();
    case CalendarNameRole:
        if (task.deviceCalendarName && !task.deviceCalendarName->empty()) {
            return QString::fromStdString(*task.deviceCalendarName);
        }
        return {};
    case IsEveningRole:
        if (task.containsTag("evening"))
            return true;

        if (task.dueDate) {
            const auto timeT = std::chrono::system_clock::to_time_t(*task.dueDate);
            const QDateTime dt = QDateTime::fromSecsSinceEpoch(timeT);
            return dt.time().hour() >= 17;
        }
        return false;
    case NeedsSyncToServerRole:
        return task.needsSyncToServer;
    case RevisionRole:
        return task.revision;
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
    roles[NeedsSyncToServerRole] = "needsSyncToServer";
    roles[RevisionRole] = "revision";
    return roles;
}

int TaskModel::count() const
{
    return rowCount();
}

void TaskModel::reload()
{
    beginResetModel();
    P_LOG_INFO("numTasks = {}", static_cast<int>(localData().taskCount()));
    endResetModel();
    emit countChanged();
}

const core::Task *TaskModel::taskAt(int row) const
{
    if (row < 0 || row >= static_cast<int>(localData().taskCount())) // NOLINT
        return nullptr;
    return &(localData().taskAt(row));
}

const core::Task *TaskModel::taskForUuid(const QString &taskUuid) const
{
    return localData().taskForUuid(taskUuid.toStdString());
}

int TaskModel::indexForTask(const QString &taskUuid) const
{
    const auto uuidStr = taskUuid.toStdString();
    const auto &tasks = localData().data()._data.tasks;
    for (size_t i = 0; i < tasks.size(); ++i) {
        if (tasks[i].uuid == uuidStr) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

const core::LocalData &TaskModel::localData() const
{
    return dataController()->localData();
}

core::LocalData &TaskModel::localData()
{
    return dataController()->localData();
}

DataController *TaskModel::dataController() const
{
    auto *guiController = GuiController::instance();
    return guiController->dataController();
}

void TaskModel::addTask(const core::Task &task)
{
    const int numTasks = static_cast<int>(localData().taskCount());
    beginInsertRows(QModelIndex(), numTasks, numTasks);
    dataController()->addTask(task);
    endInsertRows();
    emit countChanged();
}

void TaskModel::setTaskDone(const QString &taskUuid, bool isDone)
{
    const auto *task = taskForUuid(taskUuid);
    if (task == nullptr) {
        P_LOG_ERROR("Invalid task UUID: {}", taskUuid);
        return;
    }

    if (task->isDone == isDone) {
        return;
    }

    const int idx = indexForTask(taskUuid);
    if (idx == -1) {
        P_LOG_ERROR("Failed to find index for task UUID: {}", taskUuid);
        return;
    }

    core::Task updatedTask = *task;
    updatedTask.isDone = isDone;

    dataController()->updateTask(updatedTask);
    emit dataChanged(index(idx), index(idx));
}

TaskModel::~TaskModel()
{
    P_LOG_DEBUG("~TaskModel");
}
