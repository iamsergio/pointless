// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <QObject>
#include <QString>

#include <QtQml/qqmlregistration.h>

class ErrorController : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QString errorText READ errorText WRITE setErrorText NOTIFY errorTextChanged)
    Q_PROPERTY(QString loginError READ loginError NOTIFY loginErrorChanged)
public:
    explicit ErrorController(QObject *parent = nullptr);

    [[nodiscard]] QString errorText() const;
    void setErrorText(const QString &errorText);
    [[nodiscard]] QString loginError() const;
    void setLoginError(const QString &error);

Q_SIGNALS:
    void errorTextChanged();
    void loginErrorChanged();

private:
    QString _errorText;
    QString _loginError;
};
