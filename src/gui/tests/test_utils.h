// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "gui/application.h"
#include "core/logger.h"

#include <QtGlobal>
#include <QCoreApplication>

using namespace pointless;

Application::Options initPlatform()
{
    if (std::getenv("QT_QPA_PLATFORM") == nullptr) {
        qputenv("QT_QPA_PLATFORM", "offscreen");
    }

    core::Logger::setWarningsFatal(true);
    QCoreApplication::setOrganizationName("com.pointless.tests");

    return Application::Option::GuiTests;
}
