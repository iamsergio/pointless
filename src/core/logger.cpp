// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "logger.h"
#include "core/utils.h"

#include <spdlog/sinks/stdout_color_sinks.h>

using namespace pointless::core;

std::shared_ptr<spdlog::logger> Logger::s_logger = nullptr;

void Logger::initialize()
{
    if (s_logger != nullptr) {
        return;
    }

    s_logger = spdlog::stdout_color_mt("pointless");
}

std::shared_ptr<spdlog::logger> Logger::getLogger()
{
    if (s_logger == nullptr) {
        initialize();
    }
    return s_logger;
}

void Logger::initLogLevel()
{
    bool useVerbose = std::getenv("VERBOSE") != nullptr;
#ifdef POINTLESS_DEVELOPER_MODE
    useVerbose = useVerbose || pointless::isIOS();
#endif

    if (useVerbose) {
        P_LOG_INFO("VERBOSE environment variable is set, using debug log level");
        Logger::getLogger()->set_level(spdlog::level::debug);
    }
}
