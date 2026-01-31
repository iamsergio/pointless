// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "local_settings.h"
#include "core/logger.h"

#include <QSettings>

LocalSettings::LocalSettings()
{
    P_LOG_INFO("refresh token={}", maskedToken(refreshToken()));
}

void LocalSettings::save()
{
    _settings.sync();
}

void LocalSettings::setAccessToken(const std::string &token)
{
    _settings.setValue("auth/accessToken", QString::fromStdString(token));
}

std::string LocalSettings::accessToken() const
{
    return _settings.value("auth/accessToken").toString().toStdString();
}

void LocalSettings::setRefreshToken(const std::string &token)
{
    P_LOG_INFO("auth/refreshToken={}", maskedToken(token));
    _settings.setValue("auth/refreshToken", QString::fromStdString(token));
}

std::string LocalSettings::refreshToken() const
{
    return _settings.value("auth/refreshToken").toString().toStdString();
}

void LocalSettings::setUserId(const std::string &userId)
{
    P_LOG_INFO("auth/userId={}", userId);
    _settings.setValue("auth/userId", QString::fromStdString(userId));
}

std::string LocalSettings::userId() const
{
    return _settings.value("auth/userId").toString().toStdString();
}

void LocalSettings::clear()
{
    _settings.remove("auth/accessToken");
    _settings.remove("auth/refreshToken");
    _settings.remove("auth/userId");
    save();
}

std::string LocalSettings::maskedToken(const std::string &token) const
{
    return token.substr(0, 6);
}
