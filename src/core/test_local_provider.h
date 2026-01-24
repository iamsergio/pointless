// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include "data_provider.h"

#include <string>

class TestLocalDataProvider : public IDataProvider
{
public:
    explicit TestLocalDataProvider(std::string filePath);

    bool login(const std::string &email, const std::string &password) override;
    bool loginWithDefaults() override;
    [[nodiscard]] std::pair<std::string, std::string> defaultLoginPassword() const override;
    void logout() override;

    [[nodiscard]] bool isAuthenticated() override;
    std::expected<std::string, TraceableError> pullData() override;
    std::expected<void, TraceableError> pushData(const std::string &data) override;

private:
    std::string _filePath;
};
