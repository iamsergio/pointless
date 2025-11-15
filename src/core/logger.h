// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <quill/Backend.h>
#include <quill/Frontend.h>
#include <quill/LogMacros.h>
#include <quill/sinks/ConsoleSink.h>

class Logger
{
public:
    static void initialize();
    static quill::Logger *getLogger();

private:
    static quill::Logger *s_logger;
    static bool s_initialized;
};

#define P_LOG_DEBUG(...) LOG_DEBUG(Logger::getLogger(), __VA_ARGS__)
#define P_LOG_INFO(...) LOG_INFO(Logger::getLogger(), __VA_ARGS__)
#define P_LOG_WARNING(...) LOG_WARNING(Logger::getLogger(), __VA_ARGS__)
#define P_LOG_ERROR(...) LOG_ERROR(Logger::getLogger(), __VA_ARGS__)
#define P_LOG_CRITICAL(...) LOG_CRITICAL(Logger::getLogger(), __VA_ARGS__)
