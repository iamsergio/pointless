// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include "data.h"

#include <expected>
#include <string>
#include <variant>

namespace pointless::core {

class LocalData
{
public:
    LocalData();

    [[nodiscard]] std::expected<std::monostate, std::string> loadDataFromFile();
    [[nodiscard]] std::expected<Data, std::string> loadDataFromFile(const std::string &filename) const;

    [[nodiscard]] const Data &getData() const
    {
        return _data;
    }

private:
    [[nodiscard]] std::string getDataFilePath() const;
    std::string _dataDir;
    Data _data;
};

}
