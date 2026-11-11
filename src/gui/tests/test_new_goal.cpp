// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "gui_test_harness.h"
#include "core/logger.h"
#include "core/data_provider.h"
#include "core/context.h"
#include "core/local_data.h"

#include <gtest/gtest.h>

#include <chrono>
#include <string>

using namespace pointless;
using pointless::tests::GuiTestHarness;

static int g_argc;
static char **g_argv;

static const std::string testDataPath = std::string(POINTLESS_SOURCE_DIR) + "/src/gui/tests/test_data.json";

static const std::string s_titleMatchingTag = "work";

class NewGoalTest : public spix::TestServer
{
public:
    explicit NewGoalTest(GuiTestHarness &harness)
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

        mouseClick("mainWindow/goalsViewButton");
        wait(std::chrono::milliseconds(300));
        ASSERT_TRUE(existsAndVisible("mainWindow/goalsView"));
        ASSERT_EQ(getStringProperty("mainWindow/goalsView", "count"), "0");

        mouseClick("mainWindow/addTaskButton");
        wait(std::chrono::milliseconds(300));
        ASSERT_TRUE(existsAndVisible("mainWindow/editTask"));
        EXPECT_EQ(getStringProperty("mainWindow/editTask", "isGoal"), "true");

        setStringProperty("mainWindow/editTask/titleInput", "text", s_titleMatchingTag);
        mouseClick("mainWindow/editTask/saveButton");
        wait(std::chrono::milliseconds(300));
        ASSERT_FALSE(existsAndVisible("mainWindow/editTask"));

        bool isGoal = false;
        _harness.runOnGuiThread([this, &isGoal] {
            const auto *task = _harness.localData().taskForTitle(s_titleMatchingTag);
            isGoal = task != nullptr && task->isGoal.value_or(false);
        });
        EXPECT_TRUE(isGoal);

        EXPECT_EQ(getStringProperty("mainWindow/goalsView", "count"), "1");
        ASSERT_TRUE(existsAndVisible("mainWindow/goalsView/task_0"));
        EXPECT_EQ(getStringProperty("mainWindow/goalsView/task_0", "text"), s_titleMatchingTag);
    }

private:
    GuiTestHarness &_harness;
};

TEST(NewGoal, titleMatchingATagIsSaved)
{
    GuiTestHarness::setTestNow(2025, 12, 1, 16, 0);
    core::Context::setContext({ IDataProvider::Type::TestsLocal, testDataPath, static_cast<unsigned int>(core::Context::StartupOption::RestoreAuth), true });

    GuiTestHarness harness(g_argc, g_argv);
    core::Logger::setWarningsFatal(false);
    NewGoalTest testServer(harness);
    EXPECT_NO_THROW(harness.run(testServer));
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    g_argc = argc;
    g_argv = argv;
    return RUN_ALL_TESTS();
}
