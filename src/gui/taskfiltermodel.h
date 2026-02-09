// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <QSortFilterProxyModel>
#include <QtQml/qqmlregistration.h>
#include <QDate>

class GuiController;

class TaskFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(TaskFilterModel::ViewType viewType READ viewType WRITE setViewType NOTIFY viewTypeChanged)
    Q_PROPERTY(QDate dateFilter READ dateFilter WRITE setDateFilter NOTIFY dateFilterChanged)
    Q_PROPERTY(QString tagName READ tagName WRITE setTagName NOTIFY tagNameChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(bool isEmpty READ isEmpty NOTIFY emptyChanged)
    Q_PROPERTY(bool hideEvening READ hideEvening WRITE setHideEvening NOTIFY hideEveningChanged)
public:
    enum class ViewType : uint8_t {
        Week,
        Soon,
        Later
    };
    Q_ENUM(ViewType)

    explicit TaskFilterModel(QObject *parent = nullptr);
    ~TaskFilterModel() override = default;

    TaskFilterModel(const TaskFilterModel &) = delete;
    TaskFilterModel &operator=(const TaskFilterModel &) = delete;
    TaskFilterModel(TaskFilterModel &&) = delete;
    TaskFilterModel &operator=(TaskFilterModel &&) = delete;

    [[nodiscard]] ViewType viewType() const;
    void setViewType(ViewType type);
    Q_SIGNAL void viewTypeChanged();

    [[nodiscard]] QString tagName() const;
    void setTagName(const QString &tagName);
    Q_SIGNAL void tagNameChanged();

    [[nodiscard]] QDate dateFilter() const;
    void setDateFilter(QDate date);
    Q_SIGNAL void dateFilterChanged();

    [[nodiscard]] int count() const;
    Q_SIGNAL void countChanged();


    [[nodiscard]] bool isEmpty() const;
    Q_SIGNAL void emptyChanged();

    [[nodiscard]] bool hideEvening() const;
    void setHideEvening(bool hide);
    Q_SIGNAL void hideEveningChanged();

    [[nodiscard]] bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
    [[nodiscard]] bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;

private:
    void evaluateEmpty();

    ViewType _viewType = ViewType::Week;
    QString _tagName;
    QDate _dateFilter;
    int _previousRowCount = 0;
    bool _hideEvening = false;
};
