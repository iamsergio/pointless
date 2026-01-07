// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <glaze/glaze.hpp>

#include <string>

namespace pointless::core {

constexpr auto BUILTIN_TAG_SOON = "soon";
constexpr auto BUILTIN_TAG_CURRENT = "current";

class Tag
{
public:
    bool operator==(const Tag &other) const;
    [[nodiscard]] bool isBuiltin() const;

    int revision = -1;
    bool needsSyncToServer = false;
    std::string name;
};

bool tagIsBuiltin(const Tag &tag);
bool tagIsBuiltin(std::string_view name);

} // namespace pointless::core

template<>
struct glz::meta<pointless::core::Tag>
{
    using T = pointless::core::Tag;
    static constexpr auto value = object(
        "revision", &T::revision,
        "name", &T::name);
};
