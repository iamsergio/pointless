// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include "data.h"

#include <expected>
#include <string>

namespace pointless::core {

class LocalData
{
public:
    LocalData();

    [[nodiscard]] std::expected<pointless::core::Data, std::string> loadDataFromFile() const;
    [[nodiscard]] std::expected<pointless::core::Data, std::string> loadDataFromFile(const std::string &filename) const;

private:
    [[nodiscard]] std::string getDataFilePath() const;
    std::string _dataDir;
};

}
