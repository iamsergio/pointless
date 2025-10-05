#include "task_manager.h"
#include <gtest/gtest.h>
#include <fstream>
#include <nlohmann/json.hpp>

using namespace PointlessCore;

TEST(TaskManagerTest, LoadJsonFile)
{


    // Try to open test.json from several possible locations
    std::ifstream file;
    std::string json_path;
    const char *candidates[] = {
        "src/core/tests/test.json", // most common when running from build-dev
        "../src/core/tests/test.json", // if running from build-dev/src/core/tests
        "test.json", // if running from the test binary's directory
        "/data/sources/pointless_cpp/src/core/tests/test.json", // absolute path fallback
        "/data/sources/pointless_cpp/build-dev/src/core/tests/test.json" // absolute build path fallback
    };
    for (const char *path : candidates) {
        file.open(path);
        if (file.is_open()) {
            json_path = path;
            break;
        }
    }
    ASSERT_TRUE(file.is_open()) << "Could not open test.json in any known location";

    nlohmann::json j;
    file >> j;
    file.close();

    ASSERT_FALSE(j.empty()) << "JSON file is empty";
    ASSERT_TRUE(j.contains("tasks"));
    ASSERT_TRUE(j.contains("tags"));

    // Try to load into TaskManager
    auto managerResult = TaskManager::fromJson(j);
    ASSERT_TRUE(managerResult.has_value()) << managerResult.error();
    const TaskManager& manager = managerResult.value();
    // Check that the number of tasks and tags matches the JSON
    ASSERT_EQ(manager.taskCount(), j["tasks"].size());
    ASSERT_EQ(manager.tagCount(), j["tags"].size());
}
