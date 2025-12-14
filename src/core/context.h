// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include "data_provider.h"
#include <string>

namespace pointless::core {

struct Context
{
    IDataProvider::Type dataProviderType = IDataProvider::Type::Supabase;
    std::string localFilePath;

    static void setContext(const Context &context);
    static Context self();
};

}
