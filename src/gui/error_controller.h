// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <QObject>
#include <QString>

class ErrorController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString errorText READ errorText WRITE setErrorText NOTIFY errorTextChanged)

public:
    explicit ErrorController(QObject *parent = nullptr);

    [[nodiscard]] QString errorText() const;
    void setErrorText(const QString &errorText);

Q_SIGNALS:
    void errorTextChanged();

private:
    QString _errorText;
};
