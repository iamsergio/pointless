// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <QObject>
#include <QtQml/qqmlregistration.h>

class Controller : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(Controller::ViewType currentViewType READ currentViewType WRITE setCurrentViewType NOTIFY currentViewTypeChanged)

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

Q_SIGNALS:
    void currentViewTypeChanged();

private:
    ViewType _currentViewType = ViewType::Week;
};
