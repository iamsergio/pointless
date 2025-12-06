// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "test_supabase_provider.h"
#include "logger.h"

#include <cstdlib>

TestSupabaseProvider::TestSupabaseProvider()
    : Supabase(POINTLESS_SUPABASE_URL, POINTLESS_SUPABASE_ANON_KEY)
{
}

std::pair<std::string, std::string> TestSupabaseProvider::defaultLoginPassword() const
{
    static const std::string username = std::getenv("POINTLESS_DEBUG_USERNAME") ? std::getenv("POINTLESS_DEBUG_USERNAME") : "";
    static const std::string password = std::getenv("POINTLESS_DEBUG_PASSWORD") ? std::getenv("POINTLESS_DEBUG_PASSWORD") : "";

    if (username.empty() || password.empty()) {
        P_LOG_CRITICAL("Environment variables POINTLESS_DEBUG_USERNAME and POINTLESS_DEBUG_PASSWORD must be set for this test.");
        std::abort();
    }

    return { username, password };
}
