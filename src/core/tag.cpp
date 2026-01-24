// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "tag.h"

#include <array>
#include <string_view>
#include <algorithm>

namespace pointless::core {

bool tagIsBuiltin(const Tag &tag)
{
    return tagIsBuiltin(tag.name);
}

bool tagIsBuiltin(std::string_view name)
{
    static const std::array<std::string_view, 3> builtinTags = {
        BUILTIN_TAG_SOON,
        BUILTIN_TAG_CURRENT,
        BUILTIN_TAG_EVENING
    };

    return std::ranges::find(builtinTags, name) != builtinTags.end();
}

bool Tag::operator==(const Tag &other) const
{
    return name == other.name;
}

bool Tag::isBuiltin() const
{
    return tagIsBuiltin(name);
}

} // namespace pointless::core
