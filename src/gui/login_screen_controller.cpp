// SPDX-FileCopyrightText: 2026 Sergio Martins
// SPDX-License-Identifier: MIT

#include "login_screen_controller.h"
#include "gui_controller.h"

LoginScreenController::LoginScreenController(QObject *parent)
    : QObject(parent)
{
    auto *guiController = GuiController::instance();
    _username = guiController->defaultLoginUsername();
    connect(guiController, &GuiController::passStoreCredentialsFetched, this, &LoginScreenController::onPassStoreCredentialsFetched);
}

QString LoginScreenController::username() const
{
    return _username;
}

void LoginScreenController::setUsername(const QString &username)
{
    if (_username == username)
        return;

    _username = username;
    Q_EMIT usernameChanged();
}

QString LoginScreenController::password() const
{
    return _password;
}

void LoginScreenController::setPassword(const QString &password)
{
    if (_password == password)
        return;

    _password = password;
    Q_EMIT passwordChanged();
}

void LoginScreenController::onPassStoreCredentialsFetched(const QString &user, const QString &pass)
{
    if (!user.isEmpty())
        setUsername(user);
    if (!pass.isEmpty())
        setPassword(pass);
}
