// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "weekdayfiltermodel.h"
#include "weekdaymodel.h"
#include "taskfiltermodel.h"
#include "gui_controller.h"
#include "core/logger.h"
#include "gui/Clock.h"

#include <QAbstractItemModel>
#include <QAbstractListModel>
#include <QDate>
#include <QModelIndex>
#include <QObject>
#include <QSortFilterProxyModel>
#include <QtGlobal>

#include <cassert>

WeekdayFilterModel::WeekdayFilterModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    connect(this, &QAbstractListModel::rowsInserted, this, &WeekdayFilterModel::countChanged);
    connect(this, &QAbstractListModel::rowsRemoved, this, &WeekdayFilterModel::countChanged);
    connect(this, &QAbstractListModel::modelReset, this, &WeekdayFilterModel::countChanged);
    connect(this, &QAbstractListModel::layoutChanged, this, &WeekdayFilterModel::countChanged);

    connect(GuiController::instance(), &GuiController::showImmediateOnlyChanged, this, [this] {
#if QT_VERSION >= QT_VERSION_CHECK(6, 9, 0)
        beginFilterChange();
        endFilterChange();
#else
        invalidateFilter();
#endif
    });
}

WeekdayModel *WeekdayFilterModel::source() const
{
    return qobject_cast<WeekdayModel *>(sourceModel());
}

void WeekdayFilterModel::setSource(WeekdayModel *source)
{
    if (source == sourceModel())
        return;

    auto *oldModel = qobject_cast<WeekdayModel *>(sourceModel());
    if (oldModel != nullptr) {
        for (int i = 0; i < oldModel->rowCount(); ++i) {
            QModelIndex idx = oldModel->index(i, 0);
            auto *taskModel = oldModel->data(idx, WeekdayModel::TasksRole).value<TaskFilterModel *>();
            if (taskModel != nullptr) {
                disconnect(taskModel, &TaskFilterModel::countChanged, this, nullptr);
            } else {
                P_LOG_CRITICAL("TaskFilterModel is null at row {}", i);
            }
        }
    }

    setSourceModel(source);

    auto *weekdayModel = source;
    if (weekdayModel != nullptr) {
        const int count = weekdayModel->rowCount();
        for (int i = 0; i < count; ++i) {
            QModelIndex idx = weekdayModel->index(i, 0);
            auto *taskModel = weekdayModel->data(idx, WeekdayModel::TasksRole).value<TaskFilterModel *>();
            if (taskModel != nullptr) {
                connect(taskModel, &TaskFilterModel::emptyChanged, this, [this]() {
#if QT_VERSION >= QT_VERSION_CHECK(6, 9, 0)
                    beginFilterChange();
                    endFilterChange();
#else
                    invalidateFilter();
#endif
                });
            } else {
                P_LOG_CRITICAL("TaskFilterModel is null at row {}", i);
            }
        }
    } else {
        assert(false); // Not supported
    }

    emit sourceChanged();
}

bool WeekdayFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QModelIndex index = sourceModel()->index(source_row, 0, source_parent);

    if (GuiController::instance()->showImmediateOnly()) {
        const QDate rowDate = sourceModel()->data(index, WeekdayModel::DateRole).toDate();
        return rowDate == Gui::Clock::today();
    }

    QVariant data = sourceModel()->data(index, WeekdayModel::TasksRole);
    auto *model = data.value<TaskFilterModel *>();
    if (model == nullptr) {
        P_LOG_CRITICAL("TaskFilterModel is null at row {}", source_row);
        return false;
    }

    if (model->count() > 0) {
        return true;
    }

    const QDate rowDate = sourceModel()->data(index, WeekdayModel::DateRole).toDate();
    return Gui::Clock::today() <= rowDate;
}

int WeekdayFilterModel::count() const
{
    return rowCount();
}
