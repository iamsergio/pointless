// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "completion_log.h"
#include "Clock.h"
#include "tag.h"
#include "task.h"

#include <gtest/gtest.h>

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>

using namespace pointless::core;
using namespace std::chrono;

namespace {

class CompletionLogTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        _dir = std::filesystem::temp_directory_path() / ("pointless_completion_log_test_" + std::to_string(::getpid()));
        std::filesystem::remove_all(_dir);
        setenv("J_PATH", _dir.c_str(), 1);
    }

    void TearDown() override
    {
        Clock::reset();
        unsetenv("J_PATH");
        std::filesystem::remove_all(_dir);
    }

    static std::string readFile(const std::filesystem::path &path)
    {
        std::ifstream file(path);
        std::stringstream ss;
        ss << file.rdbuf();
        return ss.str();
    }

    std::filesystem::path _dir;
};

}

TEST_F(CompletionLogTest, FilePathUsesIsoWeek)
{
    EXPECT_EQ(CompletionLog::filePath(sys_days(April / 8 / 2026) + hours(12)), _dir / "2026" / "w15.md");
    EXPECT_EQ(CompletionLog::filePath(sys_days(April / 5 / 2026) + hours(12)), _dir / "2026" / "w14.md");
    EXPECT_EQ(CompletionLog::filePath(sys_days(April / 6 / 2026) + hours(12)), _dir / "2026" / "w15.md");
    EXPECT_EQ(CompletionLog::filePath(sys_days(December / 29 / 2025) + hours(12)), _dir / "2026" / "w1.md");
    EXPECT_EQ(CompletionLog::filePath(sys_days(January / 1 / 2027) + hours(12)), _dir / "2026" / "w53.md");
}

TEST_F(CompletionLogTest, FallsBackToHome)
{
    unsetenv("J_PATH");
    EXPECT_EQ(CompletionLog::baseDir(), std::filesystem::path(std::getenv("HOME")) / "pointless_logs");
}

TEST_F(CompletionLogTest, LineForTask)
{
    Task task;
    task.title = "Buy milk";
    EXPECT_EQ(CompletionLog::lineForTask(task), "Buy milk");

    task.tags = { pointless::core::BUILTIN_TAG_SOON, "shopping", "home" };
    EXPECT_EQ(CompletionLog::lineForTask(task), "shopping: Buy milk");
}

TEST_F(CompletionLogTest, AppendCreatesDirectoriesAndAppends)
{
    Clock::setTestNow(sys_days(April / 8 / 2026) + hours(12));

    Task first;
    first.title = "Buy milk";
    first.tags = { "shopping" };
    Task second;
    second.title = "Fix bug";
    second.tags = { "work" };

    EXPECT_TRUE(CompletionLog::append(first));
    EXPECT_TRUE(CompletionLog::append(second));

    EXPECT_EQ(readFile(_dir / "2026" / "w15.md"), "shopping: Buy milk\nwork: Fix bug\n");
}
