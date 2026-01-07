// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include "utils.h"

#include <memory>
#include <string>
#include <utility>
#include <cstdlib>

class IDataProvider
{
public:
    enum class Type : uint8_t {
        None = 0,
        TestsLocal, // tests can read from local file to make some tests faster
        Supabase, // Supabase production
        TestSupabase // Supabase with test user/password
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
    virtual std::string pullData() = 0;
    virtual bool pushData(const std::string &data) = 0;

    static std::unique_ptr<IDataProvider> createProvider();

private:
    static std::unique_ptr<IDataProvider> s_provider;
};


inline std::string_view enumToString(IDataProvider::Type type)
{
    switch (type) {
    case IDataProvider::Type::None:
        return "None";
    case IDataProvider::Type::TestsLocal:
        return "TestsLocal";
    case IDataProvider::Type::Supabase:
        return "Supabase";
    case IDataProvider::Type::TestSupabase:
        return "TestSupabase";
    }

    pointless::abort("invalid IDataProvider::Type enum value");
    return {};
}
