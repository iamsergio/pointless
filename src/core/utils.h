// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include <string>
#include <cstdlib>

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

}
