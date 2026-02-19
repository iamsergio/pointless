// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <spdlog/spdlog.h>

#include <cstdlib>
#include <memory>

namespace pointless::core {

class Logger
{
public:
    static void initialize();
    static std::shared_ptr<spdlog::logger> getLogger();
    static void initLogLevel();
    static void setWarningsFatal(bool fatal);
    static bool warningsFatal();

private:
    static std::shared_ptr<spdlog::logger> s_logger;
    static bool s_initialized;
    static bool s_warningsFatal;
};

}

// clang-format off
#define P_LOG_IMPL(level, ...) pointless::core::Logger::getLogger()->log(spdlog::source_loc { __FILE__, __LINE__, __FUNCTION__ }, level, __VA_ARGS__)
#define P_LOG_ABORT_IF_FATAL() if (pointless::core::Logger::warningsFatal()) std::abort()

#define P_LOG_DEBUG(...)             P_LOG_IMPL(spdlog::level::debug,    __VA_ARGS__)
#define P_LOG_INFO(...)              P_LOG_IMPL(spdlog::level::info,     __VA_ARGS__)
#define P_LOG_WARNING_NOABORT(...)   P_LOG_IMPL(spdlog::level::warn,     __VA_ARGS__)
#define P_LOG_ERROR_NOABORT(...)     P_LOG_IMPL(spdlog::level::err,      __VA_ARGS__)
#define P_LOG_CRITICAL_NOABORT(...)  P_LOG_IMPL(spdlog::level::critical, __VA_ARGS__)

#define P_LOG_WARNING(...)  do { P_LOG_WARNING_NOABORT(__VA_ARGS__);  P_LOG_ABORT_IF_FATAL(); } while (0)
#define P_LOG_ERROR(...)    do { P_LOG_ERROR_NOABORT(__VA_ARGS__);    P_LOG_ABORT_IF_FATAL(); } while (0)
#define P_LOG_CRITICAL(...) do { P_LOG_CRITICAL_NOABORT(__VA_ARGS__); P_LOG_ABORT_IF_FATAL(); } while (0)
// clang-format on
