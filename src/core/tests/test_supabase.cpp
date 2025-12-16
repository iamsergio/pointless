// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "../supabase.h"

#include <gtest/gtest.h>

TEST(SupabaseTest, LoginLogout)
{
    const char *username = std::getenv("POINTLESS_DEBUG_USERNAME");
    const char *password = std::getenv("POINTLESS_DEBUG_PASSWORD");
    ASSERT_NE(username, nullptr) << "POINTLESS_DEBUG_USERNAME env var not set";
    ASSERT_NE(password, nullptr) << "POINTLESS_DEBUG_PASSWORD env var not set";

    auto supabase = SupabaseProvider::createDefault();
    ASSERT_TRUE(supabase->login(username, password));
    supabase->logout();
}
