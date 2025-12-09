// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include <string>
#include <cstdlib>

namespace pointless {
inline std::string getenv_or_empty(const char *name)
{
    if (const char *v = std::getenv(name))
        return v;
    return "";
}
}
