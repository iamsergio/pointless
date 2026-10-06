// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "gui_test_harness.h"
#include "core/logger.h"
#include "core/data_provider.h"
#include "core/context.h"
#include "core/local_data.h"

#include <gtest/gtest.h>

#include <cstring>
#include <fstream>
#include <sstream>

#include <Spix/Data/ItemPath.h>

using namespace pointless;
using pointless::tests::GuiTestHarness;

static int g_argc;
static char **g_argv;

#define DEBUG_SLEEP wait(std::chrono::seconds(36000));

const std::string testDataPath = std::string(POINTLESS_SOURCE_DIR) + "/src/gui/tests/test_data.json";

class MyTest : public spix::TestServer
{
public:
    explicit MyTest(GuiTestHarness &harness)
        : _harness(harness)
    {
    }

protected:
    void executeTest() override
    {
        struct AppQuitter
        {
            ~AppQuitter()
            {
                harness.finish();
            }
            GuiTestHarness &harness;
        } appQuitter { _harness };

        P_LOG_INFO("Starting test. On secondary thread.");

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

        ASSERT_TRUE(existsAndVisible("mainWindow/weekView"));

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

        int index = 0;
        for (const auto &expectedText : expectedWeekDaysText) {
            // we need to position the item into view to have it loaded
            invokeMethod("mainWindow/weekdayListView", "positionViewAtIndex", { index, 0 });
            wait(std::chrono::milliseconds(200));

            const std::string weekdayPath = "mainWindow/weekday_" + std::to_string(index);

            ASSERT_TRUE(existsAndVisible(weekdayPath)) << "Expected list view item at index " << index;
            auto prettyDate = getStringProperty(weekdayPath, "prettyDate");
            EXPECT_EQ(prettyDate, expectedText);

            ++index;
        }

        // DEBUG_SLEEP

        // Test task counts within each day
        const auto expectedTaskCounts = { 5, 0, 1, 0, 1, 0, 1 };
        index = 0;
        for (const auto &expectedCount : expectedTaskCounts) {
            invokeMethod("mainWindow/weekdayListView", "positionViewAtIndex", { index, 0 });
            wait(std::chrono::milliseconds(200));
            const std::string weekdayPath = "mainWindow/weekday_" + std::to_string(index);
            ASSERT_TRUE(existsAndVisible(weekdayPath));

            EXPECT_EQ(getStringProperty(weekdayPath, "taskCount"), std::to_string(expectedCount));

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
        EXPECT_EQ(getStringProperty("mainWindow/editTask", "visible"), "false");
        bool taskFooExists = false;
        _harness.runOnGuiThread([this, &taskFooExists] { taskFooExists = _harness.localData().taskForTitle("foo") != nullptr; });
        EXPECT_TRUE(taskFooExists);

        // -----------------------------------------
        // Test editing an existing task
        invokeMethod("mainWindow/weekdayListView", "positionViewAtIndex", { 0, 0 });
        wait(std::chrono::milliseconds(200));

        mouseClick("mainWindow/task_0_0"); // Click on "Current Task 3" (no tags)
        wait(std::chrono::milliseconds(200));

        // Open menu
        EXPECT_EQ(getStringProperty("mainWindow/taskMenu", "visible"), "true");
        mouseClick("mainWindow/editMenuItem");
        wait(std::chrono::milliseconds(200));

        // Verify editor is open and populated
        EXPECT_EQ(getStringProperty("mainWindow/editTask", "visible"), "true");
        EXPECT_EQ(getStringProperty("mainWindow/editTask/titleInput", "text"), "Current Task 3");

        // Change title and tag
        setStringProperty("mainWindow/editTask/titleInput", "text", "Current Task 3 Edited");
        mouseClick("mainWindow/editTask/tag_work"); // Select "work" tag
        mouseClick("mainWindow/editTask/saveButton");
        wait(std::chrono::milliseconds(200));

        EXPECT_EQ(getStringProperty("mainWindow/editTask", "visible"), "false");
        return;
        // Verify changes in the model
        bool taskExists = false;
        std::string taskTagName;
        _harness.runOnGuiThread([this, &taskExists, &taskTagName] {
            auto *task = _harness.localData().taskForTitle("Current Task 3 Edited");
            taskExists = task != nullptr;
            if (task)
                taskTagName = task->tagName();
        });
        ASSERT_TRUE(taskExists);
        EXPECT_EQ(taskTagName, "work");

        // Verify changes in UI
        EXPECT_EQ(getStringProperty("mainWindow/task_0_0", "title"), "Current Task 3 Edited");
        EXPECT_EQ(getStringProperty("mainWindow/task_0_0", "taskTagName"), "work");

        P_LOG_INFO("Finished test!!");
    }

private:
    GuiTestHarness &_harness;
};

TEST(DummyTest, BasicAssertions)
{
    EXPECT_STRNE("hello", "world");
    EXPECT_EQ(7 * 6, 42);

    GuiTestHarness harness(g_argc, g_argv);
    MyTest testServer(harness);
    EXPECT_NO_THROW(harness.run(testServer));
}

// TEST(OfflineMode, EnableOfflineMode)
// {
//     EXPECT_FALSE(GuiController::instance()->isOfflineMode());
//     GuiController::instance()->enableOfflineMode();
//     EXPECT_TRUE(GuiController::instance()->isOfflineMode());
// }

void initDataProvider(IDataProvider::Type providerType)
{
    GuiTestHarness::setTestNow(2025, 12, 1, 16, 0);

    if (providerType == IDataProvider::Type::TestsLocal) {
        core::Context::setContext({ IDataProvider::Type::TestsLocal, testDataPath, static_cast<unsigned int>(core::Context::StartupOption::RestoreAuth), true });
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
