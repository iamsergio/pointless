// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "core/tag.h"

#include "core/logger.h"
#include "tagmodel.h"
#include "data_controller.h"
#include "gui_controller.h"

#include <QByteArray>
#include <QHash>
#include <QModelIndex>
#include <QObject>
#include <QString>
#include <QVariant>

TagModel::TagModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int TagModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    const auto &data = localData();
    return static_cast<int>(data.tagCount());
}

QVariant TagModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= static_cast<int>(localData().tagCount()))
        return {};

    const auto &tag = localData().tagAt(index.row());

    switch (static_cast<Roles>(role)) {
    case NameRole:
        return QString::fromStdString(tag.name);
    case IsBuiltinRole:
        return tag.isBuiltin();
    }

    return {};
}

QHash<int, QByteArray> TagModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[IsBuiltinRole] = "isBuiltin";
    return roles;
}

int TagModel::count() const
{
    return rowCount();
}

void TagModel::reload()
{
    beginResetModel();
    P_LOG_INFO("numTags = {}", static_cast<int>(localData().tagCount()));
    endResetModel();
    emit countChanged();
}

const pointless::core::LocalData &TagModel::localData() const
{
    auto *guiController = GuiController::instance();
    return guiController->dataController()->localData();
}

pointless::core::LocalData &TagModel::localData()
{
    auto *guiController = GuiController::instance();
    return guiController->dataController()->localData();
}
