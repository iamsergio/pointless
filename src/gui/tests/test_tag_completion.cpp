// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "gui_test_harness.h"
#include "core/data_provider.h"
#include "core/context.h"
#include "core/local_data.h"

#include <QtCore/qnamespace.h>

#include <gtest/gtest.h>

#include <chrono>
#include <string>

using namespace pointless;
using pointless::tests::GuiTestHarness;

static int g_argc;
static char **g_argv;

static const std::string testDataPath = std::string(POINTLESS_SOURCE_DIR) + "/src/gui/tests/test_data.json";
static const spix::ItemPath s_titleInput = "mainWindow/editTask/titleInput";

class TagCompletionTest : public spix::TestServer
{
public:
    explicit TagCompletionTest(GuiTestHarness &harness)
        : _harness(harness)
    {
    }

protected:
    void pressTab(int key = Qt::Key_Tab)
    {
        enterKey(s_titleInput, key, 0);
        wait(std::chrono::milliseconds(100));
    }

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

        _harness.runOnGuiThread([this] {
            core::Tag tag;
            tag.name = "workout";
            _harness.localData().data().addTag(tag);
        });

        mouseClick("mainWindow/addTaskButton");
        wait(std::chrono::milliseconds(300));
        ASSERT_TRUE(existsAndVisible("mainWindow/editTask"));

        setStringProperty(s_titleInput, "text", "buy milk W");
        pressTab();
        EXPECT_EQ(getStringProperty(s_titleInput, "text"), "buy milk work");
        pressTab();
        EXPECT_EQ(getStringProperty(s_titleInput, "text"), "buy milk workout");
        pressTab();
        EXPECT_EQ(getStringProperty(s_titleInput, "text"), "buy milk work");
        pressTab(Qt::Key_Backtab);
        EXPECT_EQ(getStringProperty(s_titleInput, "text"), "buy milk workout");

        setStringProperty(s_titleInput, "text", "call mum :p");
        pressTab();
        EXPECT_EQ(getStringProperty(s_titleInput, "text"), "call mum :personal");

        setStringProperty(s_titleInput, "text", "call mum s");
        pressTab();
        EXPECT_EQ(getStringProperty(s_titleInput, "text"), "call mum s");

        setStringProperty(s_titleInput, "text", "buy milk w");
        pressTab();
        EXPECT_EQ(getStringProperty(s_titleInput, "text"), "buy milk work");
        mouseClick("mainWindow/editTask/saveButton");
        wait(std::chrono::milliseconds(300));
        ASSERT_FALSE(existsAndVisible("mainWindow/editTask"));

        bool hasWorkTag = false;
        _harness.runOnGuiThread([this, &hasWorkTag] {
            const auto *task = _harness.localData().taskForTitle("buy milk");
            hasWorkTag = task != nullptr && task->containsTag("work");
        });
        EXPECT_TRUE(hasWorkTag);
    }

private:
    GuiTestHarness &_harness;
};

TEST(TagCompletion, tabCompletesAndCyclesTags)
{
    GuiTestHarness::setTestNow(2025, 12, 1, 16, 0);
    core::Context::setContext({ IDataProvider::Type::TestsLocal, testDataPath, static_cast<unsigned int>(core::Context::StartupOption::RestoreAuth), true });

    GuiTestHarness harness(g_argc, g_argv);
    TagCompletionTest testServer(harness);
    EXPECT_NO_THROW(harness.run(testServer));
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    g_argc = argc;
    g_argv = argv;
    return RUN_ALL_TESTS();
}
