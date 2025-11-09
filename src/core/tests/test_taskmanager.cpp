// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "task_manager.h"

#include <gtest/gtest.h>
#include <glaze/glaze.hpp>

#include <fstream>

using namespace PointlessCore;

TEST(TaskManagerTest, LoadJsonFile)
{
    // Try to open test.json from several possible locations
    std::ifstream file;
    std::string json_path;
    std::string cmake_path = std::string(POINTLESS_SOURCE_DIR) + "/src/core/tests/test.json";
    const char *candidates[] = {
        "src/core/tests/test.json", // most common when running from build-dev
        "../src/core/tests/test.json", // if running from build-dev/src/core/tests
        "test.json", // if running from the test binary's directory
        cmake_path.c_str(), // using cmake source dir
        "../../src/core/tests/test.json" // relative from build dir
    };
    for (const char *path : candidates) {
        file.open(path);
        if (file.is_open()) {
            json_path = path;
            break;
        }
    }
    ASSERT_TRUE(file.is_open()) << "Could not open test.json in any known location";

    // Read the entire file into a string
    std::string json_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    ASSERT_FALSE(json_content.empty()) << "JSON file is empty";

    // Try to load into TaskManager
    auto managerResult = TaskManager::fromJson(json_content);
    ASSERT_TRUE(managerResult.has_value()) << managerResult.error();
    const TaskManager &manager = managerResult.value();

    // Basic validation - we should have some tasks and tags
    EXPECT_GT(manager.taskCount(), 0) << "Should have at least one task in test data";
}
