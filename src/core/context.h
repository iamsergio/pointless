// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include "data_provider.h"

#include <string>
#include <optional>

namespace pointless::core {

struct Context
{
    enum class StartupOption : uint8_t {
        None = 0,
        RestoreAuth = 1
    };
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

    [[nodiscard]] unsigned int startupOptions() const
    {
        return _startupOptions;
    }

    [[nodiscard]] bool shouldRestoreAuth() const
    {
        return (_startupOptions & static_cast<unsigned int>(StartupOption::RestoreAuth)) != 0U;
    }

    Context(IDataProvider::Type providerType, std::string localFilePath, unsigned int startupOptions = static_cast<unsigned int>(StartupOption::RestoreAuth));
    Context(const Context &) = default;
    Context(Context &&) = delete;
    Context &operator=(Context &&) = delete;
    Context &operator=(const Context &) = default;
    ~Context() = default;

    [[nodiscard]] static std::string clientDataDir();
    static void setClientDataDir(const std::string &dir);
    [[nodiscard]] static Context defaultContextForSupabaseTesting(unsigned int startupOptions = static_cast<unsigned int>(StartupOption::RestoreAuth));
    [[nodiscard]] static Context defaultContextForSupabaseRelease(unsigned int startupOptions = static_cast<unsigned int>(StartupOption::RestoreAuth));

private:
    IDataProvider::Type _dataProviderType;
    std::string _localFilePath;
    unsigned int _startupOptions;
    static std::optional<std::string> _clientDataDir;
};

}
