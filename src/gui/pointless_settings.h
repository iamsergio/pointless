// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <QSettings>
#include <QString>

namespace pointless {

inline constexpr auto settingsApplicationName = "pointless";

void setSettingsIdentity(const QString &organization, const QString &application);

[[nodiscard]] QSettings createSettings();

}
