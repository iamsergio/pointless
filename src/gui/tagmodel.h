// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include "core/tag.h"

#include <QAbstractListModel>
#include <QtQml/qqmlregistration.h>

#include <cstdint>
#include <vector>

class TagModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum Roles : std::uint16_t {
        NameRole = Qt::UserRole + 1,
        IsBuiltinRole
    };

    explicit TagModel(QObject *parent = nullptr);
    explicit TagModel(const std::vector<PointlessCore::Tag> &tags, QObject *parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    [[nodiscard]] int count() const;

    void setTags(const std::vector<PointlessCore::Tag> &tags);

Q_SIGNALS:
    void countChanged();

private:
    std::vector<PointlessCore::Tag> _tags;
};
