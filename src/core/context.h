// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include "data_provider.h"

#include <string>

namespace pointless::core {

struct Context
{
    static void setContext(const Context &context);
    static Context self();
    static bool hasContext();

    [[nodiscard]] IDataProvider::Type dataProviderType() const
    {
        return _dataProviderType;
    }

    [[nodiscard]] std::string localFilePath() const
    {
        return _localFilePath;
    }

    Context(IDataProvider::Type providerType, std::string localFilePath);
    Context(const Context &) = default;
    Context(Context &&) = delete;
    Context &operator=(Context &&) = delete;
    Context &operator=(const Context &) = default;
    ~Context() = default;

    [[nodiscard]] static Context defaultContextForSupabaseTesting();
    [[nodiscard]] static Context defaultContextForSupabaseRelease();

private:
    IDataProvider::Type _dataProviderType;
    std::string _localFilePath;
};

}
