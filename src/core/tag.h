// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <nlohmann/json.hpp>
#include <string>

namespace PointlessCore {

class Tag
{
public:
    Tag() = default;
    explicit Tag(const std::string &name);

    const std::string &name() const;
    bool operator==(const Tag &other) const;
    bool isBuiltin() const;
    nlohmann::json toJson() const;
    static Tag fromJson(const nlohmann::json &j);

private:
    std::string m_name;
};

} // namespace PointlessCore
