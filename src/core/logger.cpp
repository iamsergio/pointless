// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "logger.h"

using namespace pointless::core;

quill::Logger *Logger::s_logger = nullptr;

void Logger::initialize()
{
    if (s_logger != nullptr) {
        return;
    }

    quill::Backend::start();

    auto console_sink = quill::Frontend::create_or_get_sink<quill::ConsoleSink>("console_sink");
    s_logger = quill::Frontend::create_or_get_logger("pointless", console_sink);
}

quill::Logger *Logger::getLogger()
{
    if (s_logger == nullptr) {
        initialize();
    }
    return s_logger;
}

void Logger::initLogLevel()
{
    if (std::getenv("VERBOSE") != nullptr) {
        P_LOG_INFO("VERBOSE environment variable is set, using debug log level");
        Logger::getLogger()->set_log_level(quill::LogLevel::Debug);
    }
}
