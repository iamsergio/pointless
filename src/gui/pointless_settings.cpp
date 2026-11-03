// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "pointless_settings.h"

namespace {
constexpr auto s_defaultOrganization = "com.iamsergio";

QString &organization()
{
    static QString s_organization = QString::fromLatin1(s_defaultOrganization);
    return s_organization;
}

QString &application()
{
    static QString s_application = QString::fromLatin1(pointless::settingsApplicationName);
    return s_application;
}
}

void pointless::setSettingsIdentity(const QString &org, const QString &app)
{
    organization() = org;
    application() = app;
}

QSettings pointless::createSettings()
{
    return QSettings(organization(), application());
}
