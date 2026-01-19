// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "test_server.h"
#include "gui/Clock.h"
#include "gui/spix_utils.h"
#include "gui/gui_controller.h"
#include "gui/data_controller.h"
#include "core/logger.h"
#include "core/data_provider.h"
#include "core/context.h"
#include "core/local_data.h"

#include <gtest/gtest.h>

#include <cstring>
#include <fstream>
#include <sstream>

#include <Spix/QtQmlBot.h>
#include <Spix/Data/ItemPath.h>

using namespace pointless;

static int g_argc;
static char **g_argv;

const std::string testDataPath = std::string(POINTLESS_SOURCE_DIR) + "/src/gui/tests/test_data.json";

class MyTest : public pointless::TestServer
{
public:
    using pointless::TestServer::TestServer;

protected:
    void executeTest() override
    {
        struct AppQuitter
        {
            ~AppQuitter()
            {
                qApp->quit();
            }
        } appQuitter;


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
        EXPECT_EQ(getStringProperty("mainWindow/weekView", "weekdayFilterModelCount"), "7");

        // Test header text
        const auto expectedWeekDaysText = { "MONDAY, 1",
                                            "TUESDAY, 2",
                                            "WEDNESDAY, 3",
                                            "THURSDAY, 4",
                                            "FRIDAY, 5",
                                            "SATURDAY, 6",
                                            "SUNDAY, 7" };
        const auto indexesContainingTasks = { 0, 2, 4, 6 };



        int index = 0;
        for (const auto &expectedText : expectedWeekDaysText) {
            // we need to position the item into view to have it loaded
            invokeMethod("mainWindow/weekdayListView", "positionViewAtIndex", { index, 0 });
            wait(std::chrono::milliseconds(200));

            auto listViewItem = SpixUtils::getListViewItemAtIndex(spix::ItemPath("mainWindow/weekdayListView"), index);

            ASSERT_NE(listViewItem, nullptr) << "Expected list view item at index " << index << listViewItem;
            auto prettyDate = listViewItem->property("prettyDate").toString();
            EXPECT_EQ(prettyDate, expectedText);

            ++index;
        }

        // Test task counts within each day
        const auto expectedTaskCounts = { 5, 0, 1, 0, 1, 0, 1 };
        index = 0;
        for (const auto &expectedCount : expectedTaskCounts) {
            invokeMethod("mainWindow/weekdayListView", "positionViewAtIndex", { index, 0 });
            wait(std::chrono::milliseconds(200));
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
        EXPECT_EQ(getStringProperty("mainWindow/soonView", "count"), "7");

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

        // -----------------------------------------
        // Test adding a new task via plus button that's in the week view
        mouseClick("mainWindow/weekViewButton");
        wait(std::chrono::milliseconds(200));

        invokeMethod("mainWindow/weekdayListView", "positionViewAtIndex", { 2, 0 });
        wait(std::chrono::milliseconds(200));
        mouseClick("mainWindow/addTaskButton_2");
        wait(std::chrono::milliseconds(200));

        EXPECT_EQ(getStringProperty("mainWindow/editTask", "visible"), "true");
        EXPECT_EQ(getStringProperty("mainWindow/editTask/titleInput", "focus"), "true");
        EXPECT_EQ(getStringProperty("mainWindow/editTask/titleInput", "text"), "");

        setStringProperty("mainWindow/editTask/titleInput", "text", "foo");

        mouseClick("mainWindow/editTask/saveButton");
        wait(std::chrono::milliseconds(200));
        auto &localData = GuiController::instance()->dataController()->localData();
        localData.data().needsLocalSave = false; // prevent changing test data
        EXPECT_EQ(getStringProperty("mainWindow/editTask", "visible"), "false");
        EXPECT_TRUE(localData.taskForTitle("foo") != nullptr);

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

void initDataProvider(IDataProvider::Type providerType)
{
    Gui::Clock::setTestNow(QDateTime(QDate(2025, 12, 1), QTime(16, 0)));

    if (providerType == IDataProvider::Type::TestsLocal) {
        core::Context::setContext({ IDataProvider::Type::TestsLocal, testDataPath });
    } else if (providerType == IDataProvider::Type::TestSupabase) {
        core::Context::setContext(core::Context::defaultContextForSupabaseTesting());
        auto provider = IDataProvider::createProvider();
        if (!provider->loginWithDefaults()) {
            P_LOG_CRITICAL("Failed to login to Supabase for test initialization");
            std::abort();
        }

        std::ifstream t(testDataPath);
        if (!t.is_open()) {
            P_LOG_CRITICAL("Failed to open test data file: {}", testDataPath);
            std::abort();
        }
        std::stringstream buffer;
        buffer << t.rdbuf();
        std::string jsonContent = buffer.str();

        auto result = provider->pushData(jsonContent);
        if (!result) {
            P_LOG_CRITICAL("Failed to update Supabase with test data: {}", result.error().toString());
            std::abort();
        }

        provider->logout();
    } else {
        std::abort();
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    IDataProvider::Type providerType = {};

    int newArgc = 1;
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--local") == 0) {
            providerType = IDataProvider::Type::TestsLocal;
        } else if (std::strcmp(argv[i], "--supabase") == 0) {
            providerType = IDataProvider::Type::TestSupabase;
        } else {
            argv[newArgc++] = argv[i];
        }
    }
    argv[newArgc] = nullptr;
    argc = newArgc;

    if (providerType == IDataProvider::Type::None) {
        P_LOG_ERROR("Usage: {} --local | --supabase", argv[0]);
        return 1;
    }

    initDataProvider(providerType);

    g_argc = argc;
    g_argv = argv;

    return RUN_ALL_TESTS();
}
