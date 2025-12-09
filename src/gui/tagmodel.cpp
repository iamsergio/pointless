// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT


#include <vector>

#include <QByteArray>
#include <QHash>
#include <QModelIndex>
#include <QObject>
#include <QString>
#include <QVariant>
#include <QtGlobal> // Q_UNUSED

#include "../core/tag.h"
#include "logger.h"
#include "tagmodel.h"

TagModel::TagModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

TagModel::TagModel(const std::vector<PointlessCore::Tag> &tags, QObject *parent)
    : QAbstractListModel(parent)
    , _tags(tags)
{
}

int TagModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return static_cast<int>(_tags.size());
}

QVariant TagModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= static_cast<int>(_tags.size()))
        return {};

    const auto &tag = _tags[index.row()];

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

void TagModel::setTags(const std::vector<PointlessCore::Tag> &tags)
{
    beginResetModel();
    _tags = tags;
    P_LOG_INFO("size = {}", static_cast<int>(_tags.size()));
    endResetModel();
    emit countChanged();
}
