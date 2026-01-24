// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "gui/local_settings.h"

#include <gtest/gtest.h>
#include <QCoreApplication>

static int g_argc;
static char **g_argv;

TEST(LocalSettingsTest, Persistence)
{
    {
        LocalSettings settings;
        settings.clear();
        settings.save();

        settings.setAccessToken("token123");
        settings.setUserId("user456");
        settings.setRefreshToken("refresh789");
        settings.save();
    }

    {
        LocalSettings settings;
        // Constructing a new instance should theoretically see the changes if they were persisted.
        // QSettings acts as a singleton-ish or shared state if backed by same file.

        EXPECT_EQ(settings.accessToken(), "token123");
        EXPECT_EQ(settings.userId(), "user456");
        EXPECT_EQ(settings.refreshToken(), "refresh789");

        // Cleanup
        settings.clear();
        settings.save();
    }

    // Verify cleanup
    {
        LocalSettings settings;
        EXPECT_TRUE(settings.accessToken().empty());
        EXPECT_TRUE(settings.userId().empty());
        EXPECT_TRUE(settings.refreshToken().empty());
    }
}

int main(int argc, char **argv)
{
    g_argc = argc;
    g_argv = argv;
    QCoreApplication app(g_argc, g_argv);
    app.setOrganizationName("PointlessTest");
    app.setApplicationName("LocalSettingsTest");

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
