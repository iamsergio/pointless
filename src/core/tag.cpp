// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "tag.h"

namespace PointlessCore {

bool Tag::operator==(const Tag &other) const
{
    return name == other.name;
}

bool Tag::isBuiltin() const
{
    return !name.empty() && name[0] == '_';
}

} // namespace PointlessCore
