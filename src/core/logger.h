// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <quill/Backend.h>
#include <quill/Frontend.h>
#include <quill/LogMacros.h>
#include <quill/sinks/ConsoleSink.h>

namespace pointless::core {

class Logger
{
public:
    static void initialize();
    static quill::Logger *getLogger();
    static void initLogLevel();

private:
    static quill::Logger *s_logger;
    static bool s_initialized;
};

}

#define P_LOG_DEBUG(...) LOG_DEBUG(pointless::core::Logger::getLogger(), __VA_ARGS__)
#define P_LOG_INFO(...) LOG_INFO(pointless::core::Logger::getLogger(), __VA_ARGS__)
#define P_LOG_WARNING(...) LOG_WARNING(pointless::core::Logger::getLogger(), __VA_ARGS__)
#define P_LOG_ERROR(...) LOG_ERROR(pointless::core::Logger::getLogger(), __VA_ARGS__)
#define P_LOG_CRITICAL(...) LOG_CRITICAL(pointless::core::Logger::getLogger(), __VA_ARGS__)
