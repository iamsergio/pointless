// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "gui/application.h"
#include "gui/pointless_settings.h"
#include "core/logger.h"

#include <QtGlobal>
#include <QCoreApplication>
#include <QFileInfo>
#include <QTemporaryDir>

using namespace pointless;

Application::Options initPlatform(const char *argv0)
{
    if (std::getenv("QT_QPA_PLATFORM") == nullptr) {
        qputenv("QT_QPA_PLATFORM", "offscreen");
    }

    static QTemporaryDir completionLogDir;
    qputenv("J_PATH", completionLogDir.path().toLocal8Bit());

    core::Logger::setWarningsFatal(true);
    QCoreApplication::setOrganizationName("com.pointless.tests");
    pointless::setSettingsIdentity(QStringLiteral("com.pointless.tests"), QFileInfo(QString::fromLocal8Bit(argv0)).baseName());

    return Application::Option::GuiTests;
}
