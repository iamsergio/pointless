// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "logger.h"

#include <string>
#include <string_view>
#include <cstdlib>
#include <source_location>

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

namespace pointless {
inline std::string getenv_or_empty(const char *name)
{
    if (const char *v = std::getenv(name))
        return v;
    return "";
}

inline bool isIOS()
{
#if TARGET_OS_IPHONE
    return true;
#else
    return false;
#endif
}

inline void abort(std::string_view message, std::source_location location = std::source_location::current())
{
    P_LOG_CRITICAL("aborting: {} at {}", message, location);
    core::Logger::getLogger()->flush_log();
    std::abort();
}

}
