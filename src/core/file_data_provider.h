// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include "data_provider.h"
#include <string>

class FileDataProvider : public IDataProvider
{
public:
    explicit FileDataProvider(const std::string &filePath);

    bool login(const std::string &email, const std::string &password) override;
    bool loginWithDefaults() override;
    std::pair<std::string, std::string> defaultLoginPassword() const override;
    void logout() override;

    bool isAuthenticated() const override;
    std::string retrieveData() override;
    bool updateData(const std::string &data) override;

private:
    std::string _filePath;
};
