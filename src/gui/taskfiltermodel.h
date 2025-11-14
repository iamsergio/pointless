// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <QSortFilterProxyModel>
#include <QtQml/qqmlregistration.h>

class Controller;

class TaskFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("TaskFilterModel is created by Controller")
public:
    explicit TaskFilterModel(Controller *controller, QObject *parent = nullptr);
    ~TaskFilterModel() override;

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;

private:
    Controller *_controller = nullptr;
};
