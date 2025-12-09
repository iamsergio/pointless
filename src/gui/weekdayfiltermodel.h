// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <QSortFilterProxyModel>
#include <QtQml/qqmlregistration.h>

class WeekdayFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(WeekdayFilterModel)
    QML_ELEMENT
    Q_PROPERTY(QObject *source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    explicit WeekdayFilterModel(QObject *parent = nullptr);
    ~WeekdayFilterModel() override = default;

    [[nodiscard]] QObject *source() const;
    void setSource(QObject *source);
    [[nodiscard]] int count() const;
    [[nodiscard]] bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

Q_SIGNALS:
    void sourceChanged();
    void countChanged();
};
