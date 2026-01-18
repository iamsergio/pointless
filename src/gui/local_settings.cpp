// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "local_settings.h"

#include <QSettings>

LocalSettings::LocalSettings() = default;

void LocalSettings::setAccessToken(const std::string &token)
{
    QSettings settings;
    settings.setValue("auth/accessToken", QString::fromStdString(token));
}

std::string LocalSettings::accessToken() const
{
    QSettings settings;
    return settings.value("auth/accessToken").toString().toStdString();
}

void LocalSettings::setUserId(const std::string &userId)
{
    QSettings settings;
    settings.setValue("auth/userId", QString::fromStdString(userId));
}

std::string LocalSettings::userId() const
{
    QSettings settings;
    return settings.value("auth/userId").toString().toStdString();
}

void LocalSettings::clear()
{
    QSettings settings;
    settings.remove("auth/accessToken");
    settings.remove("auth/userId");
}
