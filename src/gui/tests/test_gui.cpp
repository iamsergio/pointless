// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "test_server.h"
#include "../gui/Clock.h"
#include "../core/logger.h"
#include "../../core/data_provider.h"
#include "../../core/file_data_provider.h"
#include <gtest/gtest.h>

#include <Spix/QtQmlBot.h>
#include <Spix/Data/ItemPath.h>
#include "../../../3rdparty/spix/libs/Scenes/QtQuick/src/FindQtItem.h"

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

        auto weekActive = getStringProperty("mainWindow/weekViewButton", "isActive");
        EXPECT_EQ(weekActive, "true");

        auto soonActive = getStringProperty("mainWindow/soonViewButton", "isActive");
        EXPECT_EQ(soonActive, "false");

        auto laterActive = getStringProperty("mainWindow/laterViewButton", "isActive");
        EXPECT_EQ(laterActive, "false");

        EXPECT_EQ(getStringProperty("mainWindow/weekViewButton", "enabled"), "true");
        EXPECT_EQ(getStringProperty("mainWindow/soonViewButton", "enabled"), "true");
        EXPECT_EQ(getStringProperty("mainWindow/laterViewButton", "enabled"), "true");

        // Test that the navigator (dateRangeText) says "Dec 1 - Dec 7"
        EXPECT_EQ(getStringProperty("mainWindow/weekNavigator/dateRangeText", "text"), "Dec 1 - Dec 7");

        // Test that pressing left/right changes the dateRangeText accordingly
        mouseClick("mainWindow/weekNavigator/leftIcon");
        wait(std::chrono::milliseconds(200));
        EXPECT_EQ(getStringProperty("mainWindow/weekNavigator/dateRangeText", "text"), "Nov 24 - Nov 30");

        mouseClick("mainWindow/weekNavigator/rightIcon");
        wait(std::chrono::milliseconds(200));
        EXPECT_EQ(getStringProperty("mainWindow/weekNavigator/dateRangeText", "text"), "Dec 1 - Dec 7");

        mouseClick("mainWindow/weekNavigator/rightIcon");
        wait(std::chrono::milliseconds(200));
        EXPECT_EQ(getStringProperty("mainWindow/weekNavigator/dateRangeText", "text"), "Dec 8 - Dec 14");

        mouseClick("mainWindow/weekNavigator/leftIcon");
        wait(std::chrono::milliseconds(200));
        EXPECT_EQ(getStringProperty("mainWindow/weekNavigator/dateRangeText", "text"), "Dec 1 - Dec 7");

        spix::ItemPath path("mainWindow/weekView");

        auto window = spix::qt::GetQQuickWindowAtPath(path);
        ASSERT_NE(window, nullptr);

        auto item = spix::qt::GetQQuickItemAtPath(path);
        ASSERT_NE(item, nullptr);

        EXPECT_EQ(getStringProperty("mainWindow/weekView", "weekdayModelCount"), "7");
        EXPECT_EQ(getStringProperty("mainWindow/weekView", "weekdayFilterModelCount"), "4");


        P_LOG_INFO("Finished test!!");

        // qApp->quit();
    }
};

TEST(DummyTest, BasicAssertions)
{
    EXPECT_STRNE("hello", "world");
    EXPECT_EQ(7 * 6, 42);

    MyTest testServer(g_argc, g_argv);
    EXPECT_NO_THROW(testServer.exec());
}

void initLocalTest()
{
    std::string testDataPath = std::string(POINTLESS_SOURCE_DIR) + "/src/gui/tests/test_data.json";
    IDataProvider::setProvider(std::make_unique<FileDataProvider>(testDataPath));
    Gui::Clock::setTestNow(QDateTime(QDate(2025, 12, 1), QTime(16, 0)));
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    initLocalTest();

    g_argc = argc;
    g_argv = argv;

    return RUN_ALL_TESTS();
}
