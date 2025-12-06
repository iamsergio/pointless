// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#pragma once

#include "data_provider.h"

#include <glaze/glaze.hpp>

#include <cstdint>
#include <string>
#include <vector>

class Supabase : public IDataProvider
{
public:
    explicit Supabase(const std::string &base_url, const std::string &anon_key);

    static Supabase createDefault();
    static std::unique_ptr<Supabase> createDefaultPtr();

    bool login(const std::string &email, const std::string &password) final;
    bool loginWithDefaults() override;
    std::pair<std::string, std::string> defaultLoginPassword() const override;
    bool isAuthenticated() const override;
    void logout() override;

    bool updateData(const std::string &data) override;
    std::string retrieveData() override;

    Supabase(const Supabase &) = delete;
    Supabase &operator=(const Supabase &) = delete;
    Supabase(Supabase &&) = delete;
    Supabase &operator=(Supabase &&) = delete;

private:
    std::string _baseUrl;
    std::string _anonKey;
    std::string _accessToken;
    std::string _userId;
    std::string _defaultUser;
    std::string _defaultPassword;

    std::string retrieveRawData();

    static std::vector<uint8_t> compress(const std::string &data);
    static std::string decompress(const std::vector<uint8_t> &compressed_data);
    static std::vector<uint8_t> base64Decode(const std::string &input);
    static std::string base64Encode(const std::vector<uint8_t> &data);
};
