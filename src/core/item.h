// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

namespace PointlessCore {

class Item
{
public:
    int revision = -1;
    bool needsSyncToServer = false;

    ~Item() = default;
};

} // namespace PointlessCore
