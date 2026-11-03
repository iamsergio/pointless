// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "pointless_init.h"
#include "gui_controller.h"
#include "data_controller.h"
#include "pointless_settings.h"

#include "gui/qt_logger.h"
#include "core/context.h"
#include "core/utils.h"

#include <QStandardPaths>

using namespace pointless;

namespace {
void printDebugInfo()
{
    P_LOG_INFO("AppDataLocation: {}", createSettings().fileName().toStdString());
    P_LOG_INFO("localFile: {}", core::Context::self().localFilePath());
}
}

void pointless::initialize(const InitOptions &options)
{
    GuiController::setDebugMode(options.debug);

    core::Logger::initLogLevel();

    if (isIOS()) {
        // Set the data dir before creating context, as ctor reads the data dir
        core::Context::setClientDataDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString());
    }

    // Context might have been set already in tests
    if (!core::Context::hasContext()) {
        const auto startupOptions = static_cast<unsigned int>(options.restoreAuth ? core::Context::StartupOption::RestoreAuth : core::Context::StartupOption::None);

        core::Context::setContext(options.testSupabase ? core::Context::defaultContextForSupabaseTesting(startupOptions)
                                                       : core::Context::defaultContextForSupabaseRelease(startupOptions));
    }

    printDebugInfo();

    // Initialize the controllers and models before loading QML
    GuiController::instance();
}

void pointless::shutdown()
{
    P_LOG_INFO("Application exiting");
    GuiController::instance()->dataController()->waitForAsyncOperations();
    delete GuiController::instance();
}
