// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "test_server.h"
#include "../core/logger.h"
#include <gtest/gtest.h>

#include <Spix/QtQmlBot.h>

static int g_argc;
static char **g_argv;

class MyTest : public pointless::TestServer
{
public:
    using pointless::TestServer::TestServer;

protected:
    void executeTest() override
    {
        P_LOG_INFO("Starting test!!");

        mouseClick("mainWindow/ok_button");
        wait(std::chrono::milliseconds(200));
        auto text = getStringProperty("mainWindow/results", "text");

        P_LOG_INFO("Finished test!!");
    }
};

TEST(DummyTest, BasicAssertions)
{
    EXPECT_STRNE("hello", "world");
    EXPECT_EQ(7 * 6, 42);

    MyTest testServer(g_argc, g_argv);
    EXPECT_NO_THROW(testServer.exec());
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    g_argc = argc;
    g_argv = argv;

    return RUN_ALL_TESTS();
}
