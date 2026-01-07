// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "context.h"
#include "logger.h"
#include "utils.h"

#include <optional>
#include <filesystem>

using namespace pointless::core;

std::optional<std::string> Context::_clientDataDir = std::nullopt;

namespace {
std::optional<Context> s_currentContext; // NOLINT // clazy:exclude=non-pod-global-static
}

Context::Context(IDataProvider::Type providerType, std::string localFilePath)
    : _dataProviderType(providerType)
    , _localFilePath(std::move(localFilePath))
{
    if (!_localFilePath.empty()) {
        auto parentPath = std::filesystem::path(_localFilePath).parent_path();
        if (!parentPath.empty()) {
            std::filesystem::create_directories(parentPath);
        }
    }
}

void Context::setContext(const Context &context)
{
    s_currentContext = context;
    P_LOG_DEBUG("Context set: providerType={}, localFilePath={}", enumToString(context._dataProviderType), context._localFilePath);
}

Context Context::self()
{
    if (s_currentContext == std::nullopt) {
        pointless::abort("FATAL: Context not set");
    }

    return s_currentContext.value(); // NOLINT
}

bool Context::hasContext()
{
    return s_currentContext.has_value();
}

std::string Context::clientDataDir()
{
    if (_clientDataDir.has_value()) {
        return _clientDataDir.value();
    }

    // fallback to environment variable
    const char *envVar = std::getenv("POINTLESS_CLIENT_DATA_DIR");
    if (envVar == nullptr) {
        pointless::abort("FATAL: POINTLESS_CLIENT_DATA_DIR environment variable is not set");
    }

    return envVar;
}

void Context::setClientDataDir(const std::string &dir)
{
    _clientDataDir = dir;
}

Context Context::defaultContextForSupabaseTesting()
{
    return { IDataProvider::Type::TestSupabase, std::filesystem::path(clientDataDir()) / "debug-pointless.json" };
}

Context Context::defaultContextForSupabaseRelease()
{
    return { IDataProvider::Type::Supabase, std::filesystem::path(clientDataDir()) / "pointless.json" };
}
