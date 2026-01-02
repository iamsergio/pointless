// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <spdlog/spdlog.h>

#include <memory>

namespace pointless::core {

class Logger
{
public:
    static void initialize();
    static std::shared_ptr<spdlog::logger> getLogger();
    static void initLogLevel();

private:
    static std::shared_ptr<spdlog::logger> s_logger;
    static bool s_initialized;
};

}

#define P_LOG_DEBUG(...) pointless::core::Logger::getLogger()->debug(__VA_ARGS__)
#define P_LOG_INFO(...) pointless::core::Logger::getLogger()->info(__VA_ARGS__)
#define P_LOG_WARNING(...) pointless::core::Logger::getLogger()->warn(__VA_ARGS__)
#define P_LOG_ERROR(...) pointless::core::Logger::getLogger()->error(__VA_ARGS__)
#define P_LOG_CRITICAL(...) pointless::core::Logger::getLogger()->critical(__VA_ARGS__)
