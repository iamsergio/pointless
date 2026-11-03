// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

namespace pointless {

struct InitOptions
{
    bool debug = false;
    bool restoreAuth = true;
    bool testSupabase = false;
};

void initialize(const InitOptions &options);
void shutdown();

}
