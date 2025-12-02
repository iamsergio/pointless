// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include "data_provider.h"
#include <string>

class FileDataProvider : public IDataProvider
{
public:
    explicit FileDataProvider(const std::string &filePath);

    bool isAuthenticated() const override;
    std::string retrieveData() override;
    bool updateData(const std::string &data) override;

private:
    std::string _filePath;
};
