// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "taskfiltermodel.h"
#include "controller.h"
#include "taskmodel.h"

TaskFilterModel::TaskFilterModel(Controller *controller, QObject *parent)
    : QSortFilterProxyModel(parent)
    , _controller(controller)
{
    connect(_controller, &Controller::currentViewTypeChanged, this, [this] {
        beginFilterChange();
        endFilterChange();
    });
}

TaskFilterModel::~TaskFilterModel() = default;

bool TaskFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    Q_UNUSED(source_parent)

    auto *taskModel = qobject_cast<TaskModel *>(sourceModel());
    if (!taskModel)
        return true;

    const PointlessCore::Task *task = taskModel->taskAt(source_row);
    if (!task)
        return true;

    Controller::ViewType viewType = _controller->currentViewType();

    if (viewType == Controller::ViewType::Week) {
        return task->isCurrent();
    } else if (viewType == Controller::ViewType::Soon) {
        return task->isSoon();
    } else if (viewType == Controller::ViewType::Later) {
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
