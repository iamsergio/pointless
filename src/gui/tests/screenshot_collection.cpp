// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

// Walks through the UI and saves a screenshot of every screen into screenshots/.
// The images are the reference for the Slint port, see PORT_TODO.md.

#include "gui_test_harness.h"
#include "core/logger.h"
#include "core/data_provider.h"
#include "core/context.h"

#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>

#include <Spix/Data/ItemPath.h>

using namespace pointless;
using pointless::tests::GuiTestHarness;

static int g_argc;
static char **g_argv;

static const std::string sourceDir = POINTLESS_SOURCE_DIR;
static const std::string screenshotsDir = sourceDir + "/screenshots/" + GuiTestHarness::name();
static const std::string testDataPath = sourceDir + "/src/gui/tests/test_data.json";

class ScreenshotCollection : public spix::TestServer
{
public:
    explicit ScreenshotCollection(GuiTestHarness &harness)
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

        std::filesystem::create_directories(screenshotsDir);

        // Main views
        shot("week_view");

        mouseClick("mainWindow/soonViewButton");
        shot("soon_view");

        mouseClick("mainWindow/laterViewButton");
        shot("later_view");

        mouseClick("mainWindow/goalsViewButton");
        shot("goals_view");

        mouseClick("mainWindow/weekViewButton");
        wait(std::chrono::milliseconds(300));

        // Add task
        mouseClick("mainWindow/addTaskButton");
        shot("edit_task_new");
        mouseClick("mainWindow/editTask/backButton");
        wait(std::chrono::milliseconds(300));

        // Task menu, edit task and notes
        mouseClick("mainWindow/task_0_0");
        shot("task_menu");

        mouseClick("mainWindow/editNotesMenuItem");
        shot("notes_popup");
        closeNotesPopup();

        mouseClick("mainWindow/task_0_0");
        wait(std::chrono::milliseconds(300));
        mouseClick("mainWindow/editMenuItem");
        shot("edit_task_existing");
        mouseClick("mainWindow/editTask/backButton");
        wait(std::chrono::milliseconds(300));

        // Side panel and the pages it leads to
        mouseClick("mainWindow/burgerButton");
        _sidePanelOpen = true;
        shot("side_panel");

        pagePass("sidePanelSearch", "searchPage", "search_page");
        pagePass("sidePanelAbout", "aboutPage", "about_page");
        pagePass("sidePanelCleanup", "cleanupPage", "cleanup_page");
        pagePass("sidePanelTags", "tagsPage", "tags_page");
        pagePass("sidePanelCalendars", "calendarsPage", "calendars_page");

        // Tags: the popups and the tasks of a tag
        mouseClick("mainWindow/burgerButton");
        wait(std::chrono::milliseconds(400));
        mouseClick("mainWindow/sidePanel/sidePanelTags");
        wait(std::chrono::milliseconds(400));

        mouseClick("mainWindow/tagsPage/addTagButton");
        shot("add_tag_popup");
        mouseClick("mainWindow/tagsPage/popupCancelButton");
        wait(std::chrono::milliseconds(300));

        mouseClick("mainWindow/tagsPage/renameTag_work");
        shot("rename_tag_popup");
        mouseClick("mainWindow/tagsPage/popupCancelButton");
        wait(std::chrono::milliseconds(300));

        mouseClick("mainWindow/tagsPage/tagRow_work");
        shot("tasks_by_tag_page");
        mouseClick("mainWindow/tasksByTagPage/backButton");
        wait(std::chrono::milliseconds(300));
    }

private:
    void shot(const std::string &name)
    {
        wait(std::chrono::milliseconds(500));
        const std::string path = screenshotsDir + "/" + name + ".png";
        std::filesystem::remove(path);
        takeScreenshot("mainWindow", path);
        // Commands are queued, a blocking query returns once the screenshot was taken
        getStringProperty("mainWindow", "visible");
        EXPECT_TRUE(std::filesystem::exists(path)) << "Screenshot not created: " << path;
        P_LOG_INFO("Saved screenshot {}", path);
    }

    // Opens the side panel entry, takes the screenshot, goes back
    void pagePass(const std::string &menuItem, const std::string &page, const std::string &name)
    {
        if (!_sidePanelOpen) {
            mouseClick("mainWindow/burgerButton");
            wait(std::chrono::milliseconds(400));
        }
        mouseClick("mainWindow/sidePanel/" + menuItem);
        _sidePanelOpen = false;
        wait(std::chrono::milliseconds(400));
        shot(name);
        mouseClick("mainWindow/" + page + "/backButton");
        wait(std::chrono::milliseconds(300));
    }

    void closeNotesPopup()
    {
        // Clicking outside the popup dismisses it
        mouseClick("mainWindow/weekViewButton");
        wait(std::chrono::milliseconds(300));
    }

    GuiTestHarness &_harness;
    bool _sidePanelOpen = false;
};

TEST(Screenshots, screenshot_collection)
{
    GuiTestHarness::setTestNow(2025, 12, 1, 16, 0);
    core::Context::setContext({ IDataProvider::Type::TestsLocal, testDataPath, static_cast<unsigned int>(core::Context::StartupOption::RestoreAuth), true });

    GuiTestHarness harness(g_argc, g_argv);
    ScreenshotCollection testServer(harness);
    EXPECT_NO_THROW(harness.run(testServer));
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    g_argc = argc;
    g_argv = argv;
    return RUN_ALL_TESTS();
}
