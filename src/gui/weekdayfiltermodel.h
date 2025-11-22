// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <QSortFilterProxyModel>
#include <QtQml/qqmlregistration.h>

class WeekdayFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QObject *source READ source WRITE setSource NOTIFY sourceChanged)

public:
    explicit WeekdayFilterModel(QObject *parent = nullptr);
    ~WeekdayFilterModel() override;

    QObject *source() const;
    void setSource(QObject *source);

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

Q_SIGNALS:
    void sourceChanged();
};
