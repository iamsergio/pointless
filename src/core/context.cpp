// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "context.h"
#include "logger.h"
#include "utils.h"

#include <optional>
#include <filesystem>

using namespace pointless::core;

namespace {
std::optional<Context> s_currentContext; // NOLINT // clazy:exclude=non-pod-global-static
}

Context::Context(IDataProvider::Type providerType, std::string localFilePath)
    : _dataProviderType(providerType)
    , _localFilePath(std::move(localFilePath))
{
}

void Context::setContext(const Context &context)
{
    s_currentContext = context;
    P_LOG_DEBUG("Context set: providerType={}, localFilePath={}", enumToString(context._dataProviderType), context._localFilePath);
}

Context Context::self()
{
    if (s_currentContext == std::nullopt) {
        P_LOG_CRITICAL("FATAL: Context not set");
        Logger::getLogger()->flush_log();
        std::abort();
    }

    return s_currentContext.value();
}

bool Context::hasContext()
{
    return s_currentContext.has_value();
}

std::string Context::clientDataDir()
{
    const char *envVar = std::getenv("POINTLESS_CLIENT_DATA_DIR");
    if (envVar == nullptr) {
        P_LOG_CRITICAL("FATAL: POINTLESS_CLIENT_DATA_DIR environment variable is not set");
        std::abort();
    }

    return envVar;
}

Context Context::defaultContextForSupabaseTesting()
{
    return { IDataProvider::Type::TestSupabase, std::filesystem::path(clientDataDir()) / "debug-pointless.json" };
}

Context Context::defaultContextForSupabaseRelease()
{
    return { IDataProvider::Type::Supabase, std::filesystem::path(clientDataDir()) / "pointless.json" };
}
