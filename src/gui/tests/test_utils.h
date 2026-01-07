// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "gui/application.h"
#include "core/logger.h"

#include <QtGlobal>

using namespace pointless;

Application::Options initPlatform()
{
    if (std::getenv("QT_QPA_PLATFORM") == nullptr) {
        qputenv("QT_QPA_PLATFORM", "offscreen");
    }

    // Tests have fatal warnings
    qputenv("QT_FATAL_WARNINGS", "1");
    core::Logger::setWarningsFatal(true);

    return Application::Option::GuiTests;
}
