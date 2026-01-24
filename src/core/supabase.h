// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include "data_provider.h"

#include <glaze/glaze.hpp>

#include <cstdint>
#include <string>
#include <vector>

class SupabaseProvider : public IDataProvider
{
public:
    explicit SupabaseProvider(std::string base_url, std::string anon_key);
    ~SupabaseProvider() override = default;

    static std::unique_ptr<SupabaseProvider> createDefault();

    bool login(const std::string &email, const std::string &password) final;
    bool loginWithDefaults() override;
    [[nodiscard]] std::pair<std::string, std::string> defaultLoginPassword() const override;
    [[nodiscard]] bool isAuthenticated() override;
    void logout() override;

    [[nodiscard]] std::string accessToken() const override;
    [[nodiscard]] std::string userId() const override;
    [[nodiscard]] std::string refreshToken() const override;
    void setAccessToken(const std::string &token) override;
    void setUserId(const std::string &userId) override;
    void setRefreshToken(const std::string &token) override;

    std::expected<void, TraceableError> pushData(const std::string &data) override;
    std::expected<std::string, TraceableError> pullData() override;

    SupabaseProvider(const SupabaseProvider &) = delete;
    SupabaseProvider &operator=(const SupabaseProvider &) = delete;
    SupabaseProvider(SupabaseProvider &&) = delete;
    SupabaseProvider &operator=(SupabaseProvider &&) = delete;

private:
    std::string _baseUrl;
    std::string _anonKey;
    std::string _accessToken;
    std::string _refreshToken;
    std::string _userId;
    std::string _defaultUser;
    std::string _defaultPassword;

    std::expected<std::string, TraceableError> retrieveRawData();
    bool refreshAccessToken();

    static std::vector<uint8_t> compress(const std::string &data);
    static std::string decompress(const std::vector<uint8_t> &compressed_data);
    static std::vector<uint8_t> base64Decode(const std::string &input);
    static std::string base64Encode(const std::vector<uint8_t> &data);
};
