// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include <string>
#include <QSettings>

class LocalSettings
{
public:
    LocalSettings();
    ~LocalSettings() = default;

    void save();

    void setAccessToken(const std::string &token);
    [[nodiscard]] std::string accessToken() const;

    void setRefreshToken(const std::string &token);
    [[nodiscard]] std::string refreshToken() const;

    void setUserId(const std::string &userId);
    [[nodiscard]] std::string userId() const;

    void clear();

    LocalSettings(const LocalSettings &) = delete;
    LocalSettings &operator=(const LocalSettings &) = delete;
    LocalSettings(LocalSettings &&) = delete;
    LocalSettings &operator=(LocalSettings &&) = delete;

private:
    [[nodiscard]] std::string maskedToken(const std::string &token) const;

    QSettings _settings;
};
