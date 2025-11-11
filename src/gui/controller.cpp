// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "controller.h"

Controller::Controller(QObject *parent)
    : QObject(parent)
{
}

Controller::ViewType Controller::currentViewType() const
{
    return _currentViewType;
}

void Controller::setCurrentViewType(ViewType viewType)
{
    if (_currentViewType == viewType)
        return;

    _currentViewType = viewType;
    emit currentViewTypeChanged();
}
