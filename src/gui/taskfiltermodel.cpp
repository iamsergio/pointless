// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "taskfiltermodel.h"
#include "gui_controller.h"
#include "data_controller.h"
#include "taskmodel.h"
#include "date_utils.h"
#include "Clock.h"

#include "core/task.h"
#include "core/logger.h"

#include <QModelIndex>
#include <QObject>
#include <QSortFilterProxyModel>
#include <QDate>

TaskFilterModel::TaskFilterModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setSourceModel(GuiController::instance()->dataController()->taskModel());

    setDynamicSortFilter(true);
    setSortCaseSensitivity(Qt::CaseInsensitive);
    sort(0, Qt::AscendingOrder);

    connect(this, &QSortFilterProxyModel::rowsInserted, this, &TaskFilterModel::countChanged);
    connect(this, &QSortFilterProxyModel::rowsRemoved, this, &TaskFilterModel::countChanged);
    connect(this, &QSortFilterProxyModel::modelReset, this, &TaskFilterModel::countChanged);
    connect(this, &QSortFilterProxyModel::layoutChanged, this, &TaskFilterModel::countChanged);

    connect(this, &TaskFilterModel::countChanged, this, &TaskFilterModel::evaluateEmpty);
}

int TaskFilterModel::count() const
{
    return rowCount();
}

TaskFilterModel::ViewType TaskFilterModel::viewType() const
{
    return _viewType;
}

void TaskFilterModel::setViewType(ViewType type)
{
    if (_viewType == type) {
        return;
    }
    beginFilterChange();
    _viewType = type;
    endFilterChange();

    emit viewTypeChanged();
}

bool TaskFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    Q_UNUSED(source_parent)

    auto *taskModel = qobject_cast<TaskModel *>(sourceModel());
    if (taskModel == nullptr) {
        P_LOG_CRITICAL("TaskModel is null");
        return true;
    }

    const pointless::core::Task *task = taskModel->taskAt(source_row);
    if (task == nullptr) {
        P_LOG_CRITICAL("Task is null at row {}", source_row);
        return true;
    }

    if ((task->isDone && !task->needsSyncToServer) || task->isGoal) {
        return false;
    }

    if (_viewType == ViewType::Week && !_dateFilter.isValid()) {
        // Happens during initialization, filter will be invalidated soon
        // the return here doesn't matter much
        return true;
    }

    if (_viewType == ViewType::Week) {
        const QDate taskDueDate = Gui::DateUtils::timepointToQDate(task->dueDate);
        const bool viewIsToday = Gui::DateUtils::isToday(_dateFilter);
        const bool hasDueDate = taskDueDate.isValid();
        if (viewIsToday && task->isCurrent() && !hasDueDate) {
            return true;
        }

        const bool isOverdue = Gui::DateUtils::isOverdue(taskDueDate, Gui::Clock::today());
        if (isOverdue && viewIsToday) {
            return true;
        }

        if (!isOverdue && taskDueDate == _dateFilter) {
            return true;
        }

    } else if ((_viewType == ViewType::Soon && task->isSoon()) || (_viewType == ViewType::Later && task->isLater())) {
        if (!task->deviceCalendarUuid.has_value()) {
            // Exclude tasks synced to device calendar from Soon/Later view
            return true;
        }
    }

    return false;
}

bool TaskFilterModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    const QString leftTagName = sourceModel()->data(source_left, TaskModel::TagNameRole).toString();
    const QString rightTagName = sourceModel()->data(source_right, TaskModel::TagNameRole).toString();

    if (_viewType == ViewType::Week) {
        const bool leftIsImportant = sourceModel()->data(source_left, TaskModel::IsImportantRole).toBool();
        const bool rightIsImportant = sourceModel()->data(source_right, TaskModel::IsImportantRole).toBool();
        if (leftIsImportant != rightIsImportant) {
            return leftIsImportant;
        }

        const bool leftIsEvening = sourceModel()->data(source_left, TaskModel::IsEveningRole).toBool();
        const bool rightIsEvening = sourceModel()->data(source_right, TaskModel::IsEveningRole).toBool();
        if (leftIsEvening != rightIsEvening) {
            return !leftIsEvening;
        }
    }

    if (leftTagName != rightTagName) {
        return leftTagName < rightTagName;
    }

    return source_left.row() < source_right.row();
}

QDate TaskFilterModel::dateFilter() const
{
    return _dateFilter;
}

void TaskFilterModel::setDateFilter(QDate date)
{
    if (_dateFilter == date)
        return;
    beginFilterChange();
    _dateFilter = date;
    endFilterChange();

    setObjectName(QStringLiteral("TaskFilterModel_Date_%1").arg(_dateFilter.toString(Qt::ISODate)));

    emit dateFilterChanged();
}

void TaskFilterModel::evaluateEmpty()
{
    const int currentCount = rowCount();

    if ((currentCount == 0 && _previousRowCount > 0) || ((currentCount > 0 && _previousRowCount == 0))) {
        Q_EMIT emptyChanged();
    }

    _previousRowCount = currentCount;
}

bool TaskFilterModel::isEmpty() const
{
    return rowCount() == 0;
}
