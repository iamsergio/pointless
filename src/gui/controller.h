// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <QDate>
#include <QObject>
#include <QtQml/qqmlregistration.h>

class Controller : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(Controller::ViewType currentViewType READ currentViewType WRITE setCurrentViewType NOTIFY currentViewTypeChanged)
    Q_PROPERTY(QDate navigatorStartDate READ navigatorStartDate NOTIFY navigatorStartDateChanged)
    Q_PROPERTY(QDate navigatorEndDate READ navigatorEndDate NOTIFY navigatorEndDateChanged)

public:
    enum class ViewType {
        Week,
        Soon,
        Later
    };
    Q_ENUM(ViewType)

    explicit Controller(QObject* parent = nullptr);

    ViewType currentViewType() const;
    void setCurrentViewType(ViewType viewType);

    QDate navigatorStartDate() const;
    QDate navigatorEndDate() const;
    void setNavigatorStartDate(const QDate& date);
    Q_INVOKABLE void navigatorGotoToday();
    Q_INVOKABLE void navigatorGotoNextWeek();
    Q_INVOKABLE void navigatorGotoPreviousWeek();

Q_SIGNALS:
    void currentViewTypeChanged();
    void navigatorStartDateChanged();
    void navigatorEndDateChanged();

private:
    ViewType _currentViewType = ViewType::Week;
    QDate _navigatorStartDate;
};
