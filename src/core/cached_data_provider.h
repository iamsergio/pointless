// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

/// A provider that also caches data locally

#pragma once

#include "data_provider.h"

#include <memory>
#include <string>
#include <utility>

class CachedDataProvider : public IDataProvider
{
public:
    CachedDataProvider();
    ~CachedDataProvider() override;

    CachedDataProvider(const CachedDataProvider &) = delete;
    CachedDataProvider(CachedDataProvider &&) = delete;
    CachedDataProvider &operator=(const CachedDataProvider &) = delete;
    CachedDataProvider &operator=(CachedDataProvider &&) = delete;

    [[nodiscard]] bool isAuthenticated() const override;
    bool login(const std::string &email, const std::string &password) override;
    bool loginWithDefaults() override;
    [[nodiscard]] std::pair<std::string, std::string> defaultLoginPassword() const override;
    void logout() override;
    std::string pullData() override;
    bool pushData(const std::string &data) override;

private:
    std::unique_ptr<IDataProvider> _provider;
    const std::string _cacheFilePath;
};
