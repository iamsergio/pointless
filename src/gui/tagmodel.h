// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include "core/tag.h"

#include <QAbstractListModel>
#include <QtQml/qqmlregistration.h>

#include <cstdint>
#include <vector>

namespace pointless::core {
class LocalData;
}

class DataController;

class TagModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum Roles : std::uint16_t {
        NameRole = Qt::UserRole + 1,
        IsBuiltinRole,
        TaskCountRole
    };

    explicit TagModel(QObject *parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    [[nodiscard]] int count() const;

    void reload();

Q_SIGNALS:
    void countChanged();

private:
    [[nodiscard]] const pointless::core::LocalData &localData() const;
    [[nodiscard]] pointless::core::LocalData &localData();
    [[nodiscard]] DataController *dataController() const;
};
