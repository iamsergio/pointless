// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <QSortFilterProxyModel>
#include <QtQml/qqmlregistration.h>

class GuiController;

class TagFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    explicit TagFilterModel(QObject *parent = nullptr);
    ~TagFilterModel() override = default;

    TagFilterModel(const TagFilterModel &) = delete;
    TagFilterModel &operator=(const TagFilterModel &) = delete;
    TagFilterModel(TagFilterModel &&) = delete;
    TagFilterModel &operator=(TagFilterModel &&) = delete;

    [[nodiscard]] int count() const;
    Q_SIGNAL void countChanged();

    [[nodiscard]] bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
    [[nodiscard]] bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;
};
