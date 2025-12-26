// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "data.h"

#include <gtest/gtest.h>
#include <glaze/glaze.hpp>

#include <fstream>

using namespace pointless::core;

TEST(TaskManagerTest, LoadJsonFile)
{
    // Try to open test.json from several possible locations
    std::ifstream file;

    std::string cmake_path = std::string(POINTLESS_SOURCE_DIR) + "/src/core/tests/test.json";

    file.open(cmake_path);
    ASSERT_TRUE(file.is_open()) << "Could not open test.json at path: " << cmake_path;

    // Read the entire file into a string
    std::string json_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    ASSERT_FALSE(json_content.empty()) << "JSON file is empty";

    // Try to load into TaskManager
    auto managerResult = Data::fromJson(json_content);
    ASSERT_TRUE(managerResult.has_value()) << managerResult.error();
    const Data &manager = managerResult.value();

    // Basic validation - we should have some tasks and tags
    EXPECT_GT(manager.taskCount(), 0) << "Should have at least one task in test data";
}

TEST(TaskManagerTest, TestNullValues)
{
    // tests that when reading json files with null values, we don't crash

    std::ifstream file;
    std::string path = std::string(POINTLESS_SOURCE_DIR) + "/src/core/tests/test_null_values.json";
    file.open(path);
    ASSERT_TRUE(file.is_open()) << "Could not open test_null_values.json at path: " << path;
    std::string json_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    ASSERT_FALSE(json_content.empty()) << "JSON file is empty";
    auto managerResult = Data::fromJson(json_content);
    ASSERT_TRUE(managerResult.has_value()) << managerResult.error();
    const Data &manager = managerResult.value();
    ASSERT_EQ(manager.taskCount(), 1);
    auto tasks = manager.getAllTasks();
    ASSERT_EQ(tasks.size(), 1);
    const auto &task = tasks[0];
}
