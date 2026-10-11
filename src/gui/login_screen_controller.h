// SPDX-FileCopyrightText: 2026 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <QObject>
#include <QString>

#include <QtQml/qqmlregistration.h>

/// The state of the login screen: what is typed in the fields, and what the password store filled in.
class LoginScreenController : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
public:
    explicit LoginScreenController(QObject *parent = nullptr);

    [[nodiscard]] QString username() const;
    void setUsername(const QString &username);
    [[nodiscard]] QString password() const;
    void setPassword(const QString &password);

Q_SIGNALS:
    void usernameChanged();
    void passwordChanged();

private:
    void onPassStoreCredentialsFetched(const QString &user, const QString &pass);

    QString _username;
    QString _password;
};
