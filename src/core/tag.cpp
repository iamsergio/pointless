// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "tag.h"

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

Tag Tag::fromJson(const nlohmann::json &j)
{
    return Tag(j.at("name").get<std::string>());
}
