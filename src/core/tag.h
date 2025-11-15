// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <glaze/glaze.hpp>

#include <string>

namespace PointlessCore {

constexpr auto BUILTIN_TAG_SOON = "soon";
constexpr auto BUILTIN_TAG_CURRENT = "current";

class Tag
{
public:
    bool operator==(const Tag &other) const;
    bool isBuiltin() const;

    int revision = -1;
    bool needsSyncToServer = false;
    std::string name;
};

bool tagIsBuiltin(const Tag &tag);
bool tagIsBuiltin(const std::string_view name);

} // namespace PointlessCore

template<>
struct glz::meta<PointlessCore::Tag>
{
    using T = PointlessCore::Tag;
    static constexpr auto value = object(
        "revision", &T::revision,
        "name", &T::name);
};
