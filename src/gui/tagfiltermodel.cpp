// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "tagfiltermodel.h"
#include "gui_controller.h"
#include "data_controller.h"
#include "tagmodel.h"

#include "core/logger.h"

#include <QModelIndex>
#include <QObject>
#include <QSortFilterProxyModel>

TagFilterModel::TagFilterModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setSourceModel(GuiController::instance()->dataController()->tagModel());

    setDynamicSortFilter(true);
    setSortCaseSensitivity(Qt::CaseInsensitive);
    sort(0, Qt::AscendingOrder);

    connect(this, &QSortFilterProxyModel::rowsInserted, this, &TagFilterModel::countChanged);
    connect(this, &QSortFilterProxyModel::rowsRemoved, this, &TagFilterModel::countChanged);
    connect(this, &QSortFilterProxyModel::modelReset, this, &TagFilterModel::countChanged);
    connect(this, &QSortFilterProxyModel::layoutChanged, this, &TagFilterModel::countChanged);
}

int TagFilterModel::count() const
{
    return rowCount();
}

bool TagFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    Q_UNUSED(source_parent)

    auto *tagModel = qobject_cast<TagModel *>(sourceModel());
    if (tagModel == nullptr) {
        P_LOG_CRITICAL("TagModel is null");
        return true;
    }

    const QModelIndex index = tagModel->index(source_row, 0);
    const QString tagName = tagModel->data(index, TagModel::NameRole).toString().toLower();

    return tagName != QLatin1String("current") && tagName != QLatin1String("soon");
}

bool TagFilterModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    auto *tagModel = qobject_cast<TagModel *>(sourceModel());
    if (tagModel == nullptr) {
        P_LOG_CRITICAL("TagModel is null");
        return source_left.row() < source_right.row();
    }

    const QString leftName = tagModel->data(source_left, TagModel::NameRole).toString();
    const QString rightName = tagModel->data(source_right, TagModel::NameRole).toString();

    if (leftName == rightName) {
        return source_left.row() < source_right.row();
    }

    return leftName < rightName;
}
