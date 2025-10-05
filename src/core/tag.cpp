// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "tag.h"
#include <expected>

namespace PointlessCore {

Tag::Tag(const std::string &name)
    : m_name(name)
{
}

const std::string &Tag::name() const
{
    return m_name;
}

bool Tag::operator==(const Tag &other) const
{
    return m_name == other.m_name;
}

bool Tag::isBuiltin() const
{
    // Example: treat tags starting with '_' as builtin
    return !m_name.empty() && m_name[0] == '_';
}

nlohmann::json Tag::toJson() const
{
    return nlohmann::json { { "name", m_name } };
}

std::expected<Tag, std::string> Tag::fromJson(const nlohmann::json &j)
{
    try {
        return Tag(j.at("name").get<std::string>());
    } catch (const std::exception& e) {
        return std::unexpected<std::string>(std::string("Tag::fromJson error: ") + e.what());
    }
}

} // namespace PointlessCore
