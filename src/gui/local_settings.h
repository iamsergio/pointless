// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <string>

class LocalSettings
{
public:
    LocalSettings();
    ~LocalSettings() = default;

    void setAccessToken(const std::string &token);
    [[nodiscard]] std::string accessToken() const;

    void setUserId(const std::string &userId);
    [[nodiscard]] std::string userId() const;

    void clear();

    LocalSettings(const LocalSettings &) = delete;
    LocalSettings &operator=(const LocalSettings &) = delete;
    LocalSettings(LocalSettings &&) = delete;
    LocalSettings &operator=(LocalSettings &&) = delete;
};
