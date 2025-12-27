// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "gui/data_controller.h"
#include "core/context.h"
#include "core/data_provider.h"

#include <gtest/gtest.h>

#include <filesystem>

using namespace pointless;

constexpr const char *s_filename = "/tmp/pointless_test_data_controller_sync.json";

/// Sets the data locally and remotely before we start the test
void initData(DataController &controller, std::optional<core::Data> localData, const core::Data &remoteData)

{
    ASSERT_TRUE(controller.loginWithDefaults());

    std::filesystem::remove(s_filename);
    if (localData.has_value()) {
        auto saveResult = controller._localData.setDataAndSave(*localData);
        ASSERT_TRUE(saveResult) << "Failed to save local data: " << saveResult.error();
    }

    ASSERT_TRUE(controller.pushRemoteData(remoteData).has_value());
}

TEST(DataControllerTest, ConstructDestroy)
{
    pointless::core::Context::setContext({ IDataProvider::Type::TestSupabase, "/tmp/nonexistent_file.json" });
    DataController controller;
}

// DataController::sync() tests
TEST(DataControllerTest, Sync)
{
    pointless::core::Context::setContext({ IDataProvider::Type::TestSupabase, s_filename });
    DataController controller;

    // #2: local data doesn't exist, remote data exists -> use remote data
    core::Data remoteData;
    core::Tag tag1;
    tag1.name = "tag1";
    tag1.revision = 1;
    remoteData.addTag(tag1);

    initData(controller, {}, remoteData);
    ASSERT_FALSE(std::filesystem::exists(s_filename));
    ASSERT_TRUE(controller.refresh().has_value());

    ASSERT_TRUE(std::filesystem::exists(s_filename));

    ASSERT_EQ(controller._localData.data().allTags().size(), 1);
    EXPECT_EQ(controller._localData.data().allTags().at(0).name, "tag1");
    EXPECT_EQ(controller._localData.data().allTags().at(0).revision, 1);

    ASSERT_EQ(controller._localData.data().taskCount(), 0);
}
