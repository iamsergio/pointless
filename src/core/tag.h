// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT


#pragma once

#include <glaze/glaze.hpp>

#include <expected>
#include <string>

namespace PointlessCore {

class Tag
{
public:
    const std::string &name() const;
    bool operator==(const Tag &other) const;
    bool isBuiltin() const;

    int revision = -1;
    bool needsSyncToServer = false;
    std::string m_name;
};

} // namespace PointlessCore

template <>
struct glz::meta<PointlessCore::Tag> {
    using T = PointlessCore::Tag;
    static constexpr auto value = object(
        "revision", &T::revision,
        "needsSyncToServer", &T::needsSyncToServer,
        "name", &T::m_name
    );
};

