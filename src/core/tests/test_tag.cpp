// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "tag.h"

#include <gtest/gtest.h>
#include <glaze/glaze.hpp>

using namespace pointless::core;

TEST(TagTest, SerializeDeserializeJson)
{
    Tag original_tag;
    original_tag.revision = 42;
    original_tag.name = "test_tag";

    auto json_result = glz::write_json(original_tag);
    ASSERT_TRUE(json_result.has_value());

    std::string json_str = json_result.value();
    EXPECT_FALSE(json_str.empty());

    Tag deserialized_tag;
    auto read_error = glz::read_json(deserialized_tag, json_str);
    EXPECT_FALSE(read_error);

    EXPECT_EQ(original_tag.revision, deserialized_tag.revision);
    EXPECT_EQ(original_tag.name, deserialized_tag.name);
    EXPECT_EQ(original_tag, deserialized_tag);
}
