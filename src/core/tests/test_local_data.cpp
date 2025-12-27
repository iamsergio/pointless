// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "local_data.h"
#include "data.h"

#include <gtest/gtest.h>

#include <filesystem>
#include <cstdlib>

using namespace pointless::core;

TEST(LocalDataTest, LoadExistingFile)
{
    setenv("POINTLESS_CLIENT_DATA_DIR", "/tmp", 1);
    LocalData localData;
    std::filesystem::path testFile = std::filesystem::path(__FILE__).parent_path() / "test.json";
    auto result = localData.loadDataFromFile(testFile.string());
    ASSERT_TRUE(result.has_value());
    const Data &data = result.value();

    // Check against test.json content
    EXPECT_EQ(data.tagCount(), 3);
    EXPECT_GT(data.taskCount(), 0);
}

TEST(LocalDataTest, LoadNonExistentFile)
{
    setenv("POINTLESS_CLIENT_DATA_DIR", "/tmp", 1);
    LocalData localData;
    auto result = localData.loadDataFromFile("non_existent_file.json");
    ASSERT_TRUE(result.has_value());

    const Data &data = result.value();
    EXPECT_EQ(data.taskCount(), 0);
    EXPECT_EQ(data.tagCount(), 0);
}
