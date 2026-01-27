// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "error_controller.h"

ErrorController::ErrorController(QObject *parent)
    : QObject(parent)
{
}

QString ErrorController::errorText() const
{
    return _errorText;
}

void ErrorController::setErrorText(const QString &errorText)
{
    if (_errorText == errorText) {
        return;
    }

    _errorText = errorText;
    Q_EMIT errorTextChanged();
}
