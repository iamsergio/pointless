// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "weekdayfiltermodel.h"
#include "weekdaymodel.h"
#include "taskfiltermodel.h"
#include "../core/logger.h"

WeekdayFilterModel::WeekdayFilterModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

WeekdayFilterModel::~WeekdayFilterModel()
{
}

QObject *WeekdayFilterModel::source() const
{
    return sourceModel();
}

void WeekdayFilterModel::setSource(QObject *source)
{
    if (source == sourceModel())
        return;

    auto *oldModel = qobject_cast<WeekdayModel *>(sourceModel());
    if (oldModel) {
        for (int i = 0; i < oldModel->rowCount(); ++i) {
            QModelIndex idx = oldModel->index(i, 0);
            auto *taskModel = oldModel->data(idx, WeekdayModel::TasksRole).value<TaskFilterModel *>();
            if (taskModel) {
                disconnect(taskModel, &TaskFilterModel::countChanged, this, nullptr);
            } else {
                P_LOG_CRITICAL("TaskFilterModel is null at row {}", i);
            }
        }
    }

    auto *model = qobject_cast<QAbstractItemModel *>(source);
    setSourceModel(model);

    auto *weekdayModel = qobject_cast<WeekdayModel *>(model);
    if (weekdayModel) {
        for (int i = 0; i < weekdayModel->rowCount(); ++i) {
            QModelIndex idx = weekdayModel->index(i, 0);
            auto *taskModel = weekdayModel->data(idx, WeekdayModel::TasksRole).value<TaskFilterModel *>();
            if (taskModel) {
                connect(taskModel, &TaskFilterModel::countChanged, this, [this]() {
                    invalidateFilter();
                });
            } else {
                P_LOG_CRITICAL("TaskFilterModel is null at row {}", i);
            }
        }
    } else if (model) {
        P_LOG_CRITICAL("WeekdayFilterModel source set to a model that is not WeekdayModel");
    }

    emit sourceChanged();
}

bool WeekdayFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
    QVariant data = sourceModel()->data(index, WeekdayModel::TasksRole);
    auto *model = data.value<TaskFilterModel *>();
    if (!model)
        return false;

    return model->count() > 0;
}
