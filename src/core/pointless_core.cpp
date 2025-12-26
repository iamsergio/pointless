
// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "pointless_core.h"

#include <fstream>
#include <expected>
#include <string>
#include <iterator>

namespace pointless::core {


std::expected<Data, std::string> loadTaskManagerFromJsonFile(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file) {
        return std::unexpected("Failed to open file: " + filename);
    }

    std::string json_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    if (json_content.empty()) {
        return std::unexpected("Failed to read JSON from file: " + filename);
    }

    return Data::fromJson(json_content);
}

} // namespace pointless::core
