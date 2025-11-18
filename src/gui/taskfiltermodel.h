// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <QSortFilterProxyModel>
#include <QtQml/qqmlregistration.h>
#include <QDate>

class Controller;

class TaskFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(TaskFilterModel::ViewType viewType READ viewType WRITE setViewType NOTIFY viewTypeChanged)
    Q_PROPERTY(QDate dateFilter READ dateFilter WRITE setDateFilter NOTIFY dateFilterChanged)
public:
    enum class ViewType {
        Week,
        Soon,
        Later
    };
    Q_ENUM(ViewType)

    explicit TaskFilterModel(QObject *parent = nullptr);
    ~TaskFilterModel() override;

    ViewType viewType() const;
    void setViewType(ViewType type);
    Q_SIGNAL void viewTypeChanged();

    QDate dateFilter() const;
    void setDateFilter(const QDate &date);
    Q_SIGNAL void dateFilterChanged();

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;

private:
    ViewType _viewType = ViewType::Week;
    QDate _dateFilter;
};
