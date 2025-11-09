// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "tag.h"

namespace PointlessCore {

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
    return !m_name.empty() && m_name[0] == '_';
}

} // namespace PointlessCore
