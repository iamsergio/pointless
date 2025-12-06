// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "test_server.h"
#include "../gui/Clock.h"
#include "../gui/spix_utils.h"
#include "../core/logger.h"
#include "../../core/data_provider.h"
#include "../../core/file_data_provider.h"
#include "../../core/test_supabase_provider.h"
#include "../../core/supabase.h"

#include <gtest/gtest.h>

#include <cstring>

#include <Spix/QtQmlBot.h>
#include <Spix/Data/ItemPath.h>


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


        // Test header text
        const auto expectedWeekDaysText = { "MONDAY, 1",
                                            "WEDNESDAY, 3",
                                            "FRIDAY, 5",
                                            "SUNDAY, 7" };

        int index = 0;
        for (const auto &expectedText : expectedWeekDaysText) {
            auto listViewItem = SpixUtils::getListViewItemAtIndex(spix::ItemPath("mainWindow/weekdayListView"), index);
            ASSERT_NE(listViewItem, nullptr);

            auto prettyDate = listViewItem->property("prettyDate").toString();
            EXPECT_EQ(prettyDate, expectedText);

            ++index;
        }

        // Test task counts within each day
        const auto expectedTaskCounts = { 5, 1, 1, 1 };
        index = 0;
        for (const auto &expectedCount : expectedTaskCounts) {
            auto listViewItem = SpixUtils::getListViewItemAtIndex(spix::ItemPath("mainWindow/weekdayListView"), index);
            ASSERT_NE(listViewItem, nullptr);

            auto model = listViewItem->property("tasks").value<QObject *>();
            ASSERT_NE(model, nullptr);
            auto taskCount = model->property("count").toInt();
            EXPECT_EQ(taskCount, expectedCount);

            ++index;
        }


        // 1st task doesn't have a tag
        EXPECT_EQ(getStringProperty("mainWindow/task_0_0", "title"), "Current Task 3");
        EXPECT_EQ(getStringProperty("mainWindow/task_0_0", "taskTagName"), "");
        EXPECT_EQ(getStringProperty("mainWindow/task_0_0/tagText", "text"), "");

        // task with "work" tag is sorted a bit below
        EXPECT_EQ(getStringProperty("mainWindow/task_0_3", "taskTagName"), "work");
        EXPECT_EQ(getStringProperty("mainWindow/task_0_3/tagText", "text"), "work");
        EXPECT_EQ(getStringProperty("mainWindow/task_0_3/tagText", "visible"), "true");

        mouseClick("mainWindow/soonViewButton");
        wait(std::chrono::milliseconds(200));
        EXPECT_EQ(getStringProperty("mainWindow/soonView", "count"), "10");

        mouseClick("mainWindow/laterViewButton");
        wait(std::chrono::milliseconds(200));
        EXPECT_EQ(getStringProperty("mainWindow/laterView", "count"), "5");

        laterActive = getStringProperty("mainWindow/laterViewButton", "isActive");
        EXPECT_EQ(laterActive, "true");
        weekActive = getStringProperty("mainWindow/weekViewButton", "isActive");
        EXPECT_EQ(weekActive, "false");
        soonActive = getStringProperty("mainWindow/soonViewButton", "isActive");
        EXPECT_EQ(soonActive, "false");
        EXPECT_EQ(getStringProperty("mainWindow/weekNavigator", "visible"), "false");


        P_LOG_INFO("Finished test!!");

        qApp->quit();
    }
};

TEST(DummyTest, BasicAssertions)
{
    EXPECT_STRNE("hello", "world");
    EXPECT_EQ(7 * 6, 42);

    MyTest testServer(g_argc, g_argv);
    EXPECT_NO_THROW(testServer.exec());
}

void initDataProvider(IDataProvider::Type providerType)
{
    Gui::Clock::setTestNow(QDateTime(QDate(2025, 12, 1), QTime(16, 0)));
    if (providerType == IDataProvider::Type::Local) {
        std::string testDataPath = std::string(POINTLESS_SOURCE_DIR) + "/src/gui/tests/test_data.json";
        IDataProvider::setProvider(std::make_unique<FileDataProvider>(testDataPath));
    } else if (providerType == IDataProvider::Type::Supabase) {
        IDataProvider::setProvider(Supabase::createDefaultPtr());
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    IDataProvider::Type providerType = {};

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--local") == 0) {
            providerType = IDataProvider::Type::Local;
            break;
        } else if (std::strcmp(argv[i], "--supabase") == 0) {
            providerType = IDataProvider::Type::TestSupabase;
            break;
        }
    }

    if (providerType == IDataProvider::Type::None) {
        P_LOG_ERROR("Usage: {} --local | --supabase", argv[0]);
        return 1;
    }

    initDataProvider(providerType);

    g_argc = argc;
    g_argv = argv;

    return RUN_ALL_TESTS();
}
