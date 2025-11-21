// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "taskfiltermodel.h"
#include "controller.h"
#include "taskmodel.h"
#include "logger.h"
#include "date_utils.h"

#include <QDate>

TaskFilterModel::TaskFilterModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setSourceModel(TaskModel::instance());

    setDynamicSortFilter(true);
    setSortCaseSensitivity(Qt::CaseInsensitive);
    sort(0, Qt::AscendingOrder);

    connect(this, &QSortFilterProxyModel::rowsInserted, this, &TaskFilterModel::countChanged);
    connect(this, &QSortFilterProxyModel::rowsRemoved, this, &TaskFilterModel::countChanged);
    connect(this, &QSortFilterProxyModel::modelReset, this, &TaskFilterModel::countChanged);
    connect(this, &QSortFilterProxyModel::layoutChanged, this, &TaskFilterModel::countChanged);
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
    if (_viewType == type)
        return;
    beginFilterChange();
    _viewType = type;
    endFilterChange();

    emit viewTypeChanged();
}

TaskFilterModel::~TaskFilterModel() = default;

bool TaskFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    Q_UNUSED(source_parent)

    auto *taskModel = qobject_cast<TaskModel *>(sourceModel());
    if (!taskModel) {
        P_LOG_CRITICAL("TaskModel is null");
        return true;
    }

    const PointlessCore::Task *task = taskModel->taskAt(source_row);
    if (!task) {
        P_LOG_CRITICAL("Task is null at row {}", source_row);
        return true;
    }

    if (task->isDone || task->isGoal) {
        return false;
    }

    if (_dateFilter.isValid()) {
        if (task->dueDate.has_value()) {
            const QDate dueQDate = Gui::DateUtils::timepointToQDate(task->dueDate);
            if (Gui::DateUtils::isMonday(_dateFilter)) {
                if (dueQDate <= _dateFilter)
                    return true;
            } else {
                if (dueQDate == _dateFilter)
                    return true;
            }
            return false;
        }
    }

    if (_viewType == ViewType::Week) {
        return task->isCurrent();
    } else if (_viewType == ViewType::Soon) {
        return task->isSoon();
    } else if (_viewType == ViewType::Later) {
        return task->isLater();
    }

    return true;
}

bool TaskFilterModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    const QString leftTagName = sourceModel()->data(source_left, TaskModel::TagNameRole).toString();
    const QString rightTagName = sourceModel()->data(source_right, TaskModel::TagNameRole).toString();

    if (leftTagName != rightTagName)
        return leftTagName < rightTagName;

    return source_left.row() < source_right.row();
}

QDate TaskFilterModel::dateFilter() const
{
    return _dateFilter;
}

void TaskFilterModel::setDateFilter(const QDate &date)
{
    if (_dateFilter == date)
        return;
    beginFilterChange();
    _dateFilter = date;
    endFilterChange();

    emit dateFilterChanged();
}
