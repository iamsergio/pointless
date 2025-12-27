// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "gui/data_controller.h"
#include "core/context.h"
#include "core/data_provider.h"

#include <gtest/gtest.h>

void initData()
{
    pointless::core::Context::setContext({ IDataProvider::Type::TestSupabase, "/tmp/nonexistent_file.json" });
    DataController controller;
}

TEST(DataControllerTest, ConstructDestroy)
{
    pointless::core::Context::setContext({ IDataProvider::Type::TestSupabase, "/tmp/nonexistent_file.json" });
    DataController controller;
}
