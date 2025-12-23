// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include "task_manager.h"

#include <expected>
#include <string>

namespace pointless::core {

class LocalData
{
public:
    LocalData();

    [[nodiscard]] std::expected<PointlessCore::TaskManager, std::string> loadTaskManager() const;

private:
    [[nodiscard]] std::string getDataFilePath() const;
    std::string _dataDir;
};

}
