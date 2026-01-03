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

#define P_LOG_DEBUG(...) pointless::core::Logger::getLogger()->debug(__VA_ARGS__)
#define P_LOG_INFO(...) pointless::core::Logger::getLogger()->info(__VA_ARGS__)
#define P_LOG_WARNING(...)                                       \
    do {                                                         \
        pointless::core::Logger::getLogger()->warn(__VA_ARGS__); \
        if (pointless::core::Logger::warningsFatal())            \
            std::abort();                                        \
    } while (0)
#define P_LOG_ERROR(...)                                          \
    do {                                                          \
        pointless::core::Logger::getLogger()->error(__VA_ARGS__); \
        if (pointless::core::Logger::warningsFatal())             \
            std::abort();                                         \
    } while (0)
#define P_LOG_CRITICAL(...)                                          \
    do {                                                             \
        pointless::core::Logger::getLogger()->critical(__VA_ARGS__); \
        if (pointless::core::Logger::warningsFatal())                \
            std::abort();                                            \
    } while (0)
