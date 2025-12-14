// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <memory>
#include <string>
#include <utility>

class IDataProvider
{
public:
    enum class Type : uint8_t {
        None = 0,
        Local,
        Supabase,
        TestSupabase
    };

    virtual ~IDataProvider() = default;

    IDataProvider() = default;
    IDataProvider(const IDataProvider &) = delete;
    IDataProvider(IDataProvider &&) = delete;
    IDataProvider &operator=(const IDataProvider &) = delete;
    IDataProvider &operator=(IDataProvider &&) = delete;

    [[nodiscard]] virtual bool isAuthenticated() const = 0;
    virtual bool login(const std::string &email, const std::string &password) = 0;
    virtual bool loginWithDefaults() = 0;
    [[nodiscard]] virtual std::pair<std::string, std::string> defaultLoginPassword() const = 0;
    virtual void logout() = 0;
    virtual std::string retrieveData() = 0;
    virtual bool updateData(const std::string &data) = 0;

    static std::unique_ptr<IDataProvider> createProvider();

private:
    static std::unique_ptr<IDataProvider> s_provider;
};
