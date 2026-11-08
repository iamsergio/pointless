// SPDX-FileCopyrightText: 2026 Sergio Martins
// SPDX-License-Identifier: MIT

#include "core/data.h"

#include <QProcess>
#include <QProcessEnvironment>
#include <QTemporaryDir>

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <sstream>

using namespace pointless;

namespace {

struct RunResult
{
    int exitCode = -1;
    std::filesystem::path dataFile;
};

RunResult runPointless(const QTemporaryDir &dir, const QStringList &args)
{
    auto env = QProcessEnvironment::systemEnvironment();
    env.insert(QStringLiteral("POINTLESS_CLIENT_DATA_DIR"), dir.path());
    env.insert(QStringLiteral("XDG_CONFIG_HOME"), dir.filePath(QStringLiteral("config")));
    env.remove(QStringLiteral("QT_QPA_PLATFORM"));
    env.remove(QStringLiteral("DISPLAY"));
    env.remove(QStringLiteral("WAYLAND_DISPLAY"));

    QProcess process;
    process.setProcessEnvironment(env);
    process.setProcessChannelMode(QProcess::ForwardedChannels);
    process.start(QStringLiteral(POINTLESS_EXECUTABLE), QStringList { QStringLiteral("--test-supabase"), QStringLiteral("--no-restore-auth") } + args);

    RunResult result;
    result.dataFile = std::filesystem::path(dir.path().toStdString()) / "debug-pointless.json";
    if (process.waitForFinished(30000) && process.exitStatus() == QProcess::NormalExit)
        result.exitCode = process.exitCode();
    return result;
}

core::Data readData(const std::filesystem::path &file)
{
    std::ifstream stream(file);
    std::stringstream buffer;
    buffer << stream.rdbuf();
    auto data = core::Data::fromJson(buffer.str());
    EXPECT_TRUE(data) << data.error();
    return data.value_or(core::Data {});
}

}

TEST(CliTest, AddTaskToNewDataFile)
{
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());

    const auto result = runPointless(dir, { QStringLiteral("--add"), QStringLiteral("Buy milk") });
    ASSERT_EQ(result.exitCode, 0);
    ASSERT_TRUE(std::filesystem::exists(result.dataFile));

    const auto tasks = readData(result.dataFile).getAllTasks();
    ASSERT_EQ(tasks.size(), 1);
    EXPECT_EQ(tasks[0].title, "Buy milk");
    EXPECT_FALSE(tasks[0].uuid.empty());
    EXPECT_EQ(tasks[0].revision, -1);
    EXPECT_FALSE(tasks[0].dueDate.has_value());
    EXPECT_TRUE(tasks[0].tags.empty());
}

TEST(CliTest, AddTaskWithColonTag)
{
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());

    const auto result = runPointless(dir, { QStringLiteral("--add"), QStringLiteral("Buy milk :groceries") });
    ASSERT_EQ(result.exitCode, 0);

    const auto tasks = readData(result.dataFile).getAllTasks();
    ASSERT_EQ(tasks.size(), 1);
    EXPECT_EQ(tasks[0].title, "Buy milk");
    EXPECT_EQ(tasks[0].tags, std::vector<std::string> { "groceries" });
}

TEST(CliTest, AddTaskWithExistingTagAsLastWord)
{
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());

    core::Data existing;
    core::Tag tag;
    tag.name = "home";
    existing.addTag(tag);
    const auto json = existing.toJson();
    ASSERT_TRUE(json);
    std::ofstream(std::filesystem::path(dir.path().toStdString()) / "debug-pointless.json") << *json;

    const auto result = runPointless(dir, { QStringLiteral("--add"), QStringLiteral("Fix the sink home") });
    ASSERT_EQ(result.exitCode, 0);

    const auto tasks = readData(result.dataFile).getAllTasks();
    ASSERT_EQ(tasks.size(), 1);
    EXPECT_EQ(tasks[0].title, "Fix the sink");
    EXPECT_EQ(tasks[0].tags, std::vector<std::string> { "home" });
}

TEST(CliTest, AddTaskKeepsExistingTasks)
{
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());

    core::Data existing;
    existing.addTask(core::Task("existing-uuid", std::chrono::system_clock::now(), "Existing task"));
    const auto json = existing.toJson();
    ASSERT_TRUE(json);
    std::ofstream(std::filesystem::path(dir.path().toStdString()) / "debug-pointless.json") << *json;

    const auto result = runPointless(dir, { QStringLiteral("--add"), QStringLiteral("New task") });
    ASSERT_EQ(result.exitCode, 0);

    const auto tasks = readData(result.dataFile).getAllTasks();
    ASSERT_EQ(tasks.size(), 2);
    EXPECT_TRUE(std::ranges::any_of(tasks, [](const core::Task &t) { return t.uuid == "existing-uuid" && t.title == "Existing task"; }));
    EXPECT_TRUE(std::ranges::any_of(tasks, [](const core::Task &t) { return t.title == "New task"; }));
}

TEST(CliTest, AddEmptyTitleFails)
{
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());

    const auto result = runPointless(dir, { QStringLiteral("--add"), QStringLiteral("   ") });
    EXPECT_EQ(result.exitCode, 1);
    EXPECT_FALSE(std::filesystem::exists(result.dataFile));
}
