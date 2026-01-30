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

QString ErrorController::loginError() const
{
    return _loginError;
}

void ErrorController::setLoginError(const QString &error)
{
    if (_loginError == error) {
        return;
    }

    _loginError = error;
    Q_EMIT loginErrorChanged();
}
