// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "cached_data_provider.h"

#include "context.h"
#include "data_provider.h"
#include "logger.h"

namespace {
std::string cacheFilePath()
{
    const auto &context = pointless::core::Context::self();
    if (!context.localFilePath.empty()) {
        return context.localFilePath;
    }

    if (const char *env = std::getenv("POINTLESS_CLIENT_DATA_DIR")) {
        std::filesystem::path dir(env);
        if (!std::filesystem::exists(dir)) {
            std::error_code ec;
            std::filesystem::create_directories(dir, ec);
            if (ec) {
                P_LOG_ERROR("Failed to create POINTLESS_CLIENT_DATA_DIR directory '{}': {}", dir.string(), ec.message());
                return "pointless.json";
            }
        }
        return (dir / "pointless.json").string();
    }

    return "pointless.json";
}
}

CachedDataProvider::CachedDataProvider()
    : _provider(IDataProvider::createProvider())
    , _cacheFilePath(cacheFilePath())
{
    P_LOG_INFO("Using cache file path: {}", _cacheFilePath);
}

CachedDataProvider::~CachedDataProvider() = default;

bool CachedDataProvider::isAuthenticated() const
{
    return _provider->isAuthenticated();
}

bool CachedDataProvider::login(const std::string &email, const std::string &password)
{
    return _provider->login(email, password);
}

bool CachedDataProvider::loginWithDefaults()
{
    return _provider->loginWithDefaults();
}

std::pair<std::string, std::string> CachedDataProvider::defaultLoginPassword() const
{
    return _provider->defaultLoginPassword();
}

void CachedDataProvider::logout()
{
    _provider->logout();
}

std::string CachedDataProvider::pullData()
{
    return _provider->pullData();
}

bool CachedDataProvider::pushData(const std::string &data)
{
    return _provider->pushData(data);
}
