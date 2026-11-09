// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "gui_test_harness.h"
#include "core/logger.h"
#include "core/data_provider.h"
#include "core/context.h"

#include <gtest/gtest.h>

#include <chrono>
#include <string>
#include <vector>

using namespace pointless;
using pointless::tests::GuiTestHarness;

static int g_argc;
static char **g_argv;

static const std::string testDataPath = std::string(POINTLESS_SOURCE_DIR) + "/src/gui/tests/test_data_evening.json";

constexpr int s_morningCount = 3;
constexpr int s_eveningCount = 3;

class EveningLabelTest : public spix::TestServer
{
public:
    explicit EveningLabelTest(GuiTestHarness &harness)
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

        invokeMethod("mainWindow/weekdayListView", "positionViewAtIndex", { 0, 0 });
        wait(std::chrono::milliseconds(300));

        int morningCount = s_morningCount;
        expectSingleLabel(morningCount, "at startup");

        for (int i = 0; i < 3; ++i) {
            toggleEveningMode();
            ASSERT_EQ(taskCount(), morningCount);
            EXPECT_TRUE(visibleEveningLabels().empty());

            toggleEveningMode();
            expectSingleLabel(morningCount, "after toggling evening mode");
        }

        mouseClick("mainWindow/task_0_0");
        wait(std::chrono::milliseconds(300));
        mouseClick("mainWindow/deleteTaskMenuItem");
        wait(std::chrono::milliseconds(300));
        --morningCount;
        expectSingleLabel(morningCount, "after deleting a task above the evening tasks");

        toggleEveningMode();
        toggleEveningMode();
        expectSingleLabel(morningCount, "after toggling evening mode");

        mouseClick("mainWindow/addTaskButton_0");
        wait(std::chrono::milliseconds(300));
        setStringProperty("mainWindow/editTask/titleInput", "text", "New Task");
        mouseClick("mainWindow/editTask/saveButton");
        wait(std::chrono::milliseconds(300));
        ++morningCount;
        expectSingleLabel(morningCount, "after adding a task above the evening tasks");
    }

private:
    void toggleEveningMode()
    {
        mouseClick("mainWindow/eveningToggle");
        wait(std::chrono::milliseconds(300));
    }

    int taskCount()
    {
        return std::stoi(getStringProperty("mainWindow/weekday_0", "count"));
    }

    std::vector<int> visibleEveningLabels()
    {
        std::vector<int> result;
        const int count = taskCount();
        for (int i = 0; i < count; ++i) {
            if (existsAndVisible("mainWindow/eveningLabel_0_" + std::to_string(i)))
                result.push_back(i);
        }
        return result;
    }

    void expectSingleLabel(int morningCount, const std::string &when)
    {
        ASSERT_EQ(taskCount(), morningCount + s_eveningCount) << when;
        EXPECT_EQ(visibleEveningLabels(), std::vector<int> { morningCount }) << when;
    }

    GuiTestHarness &_harness;
};

TEST(EveningLabel, shownOnlyAboveFirstEveningTask)
{
    GuiTestHarness::setTestNow(2025, 12, 1, 10, 0);
    core::Context::setContext({ IDataProvider::Type::TestsLocal, testDataPath, static_cast<unsigned int>(core::Context::StartupOption::RestoreAuth), true });

    GuiTestHarness harness(g_argc, g_argv);
    EveningLabelTest testServer(harness);
    EXPECT_NO_THROW(harness.run(testServer));
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    g_argc = argc;
    g_argv = argv;
    return RUN_ALL_TESTS();
}
