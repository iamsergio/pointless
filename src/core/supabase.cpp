// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "supabase.h"
#include "logger.h"
#include "utils.h"

#include <cpr/cpr.h>
#include <cpr/error.h>
#include <zlib.h>

#include <array>
#include <cstdlib>
#include <memory>
#include <string>
#include <stdexcept>

namespace {

bool shouldVerifySsl()
{
#if defined(POINTLESS_DEVELOPER_MODE)
    return !pointless::isIOS();
#endif

    return true;
}

constexpr int kBufferSize = 32768;
constexpr int kHttpOk = 200;
constexpr int kHttpCreated = 201;
constexpr int kHttpNoContent = 204;
}


SupabaseProvider::SupabaseProvider(std::string base_url, std::string anon_key)
    : _baseUrl(std::move(base_url))
    , _anonKey(std::move(anon_key))
{
}
std::unique_ptr<SupabaseProvider> SupabaseProvider::createDefault()
{
#ifndef POINTLESS_SUPABASE_URL
#error "POINTLESS_SUPABASE_URL is not defined"
#endif

#ifndef POINTLESS_SUPABASE_ANON_KEY
#error "POINTLESS_SUPABASE_ANON_KEY is not defined"
#endif

    return std::make_unique<SupabaseProvider>(POINTLESS_SUPABASE_URL, POINTLESS_SUPABASE_ANON_KEY);
}

bool SupabaseProvider::login(const std::string &email, const std::string &password)
{
    const std::string auth_url = "https://" + _baseUrl + "/auth/v1/token?grant_type=password";
    const std::string body = R"({"email":")" + email + R"(","password":")" + password + R"("})";

    auto response = cpr::Post(
        cpr::Url { auth_url },
        cpr::Header {
            { "apikey", _anonKey },
            { "Content-Type", "application/json" } },
        cpr::Body { body },
        cpr::VerifySsl { shouldVerifySsl() });

    if (response.status_code != kHttpOk) {
        P_LOG_ERROR("Login failed: HTTP={} url={}", response.status_code, auth_url);
        P_LOG_ERROR("Response: text={} cpr::ErrorCode={} error.msg={}", response.text, static_cast<int>(response.error.code), response.error.message);
#ifdef POINTLESS_DEVELOPER_MODE
        P_LOG_ERROR("Request body: {}", body);
#endif
        return false;
    }

    auto json_result = glz::read_json<glz::json_t>(response.text);
    if (!json_result.has_value()) {
        P_LOG_ERROR("Failed to parse login response JSON");
        return false;
    }

    auto &json_obj = json_result.value();
    if (!json_obj.is_object()) {
        return false;
    }

    auto access_token_it = json_obj.get_object().find("access_token");
    if (access_token_it == json_obj.get_object().end() || !access_token_it->second.is_string()) {
        P_LOG_ERROR("No access_token in login response");
        return false;
    }

    auto user_it = json_obj.get_object().find("user");
    if (user_it == json_obj.get_object().end() || !user_it->second.is_object()) {
        P_LOG_ERROR("No user object in login response");
        return false;
    }

    auto &user_obj = user_it->second.get_object();
    auto id_it = user_obj.find("id");
    if (id_it == user_obj.end() || !id_it->second.is_string()) {
        P_LOG_ERROR("No user id in login response");
        return false;
    }

    _accessToken = access_token_it->second.get_string();
    _userId = id_it->second.get_string();


    P_LOG_DEBUG("Logged in with user={}", _userId);

    return true;
}

bool SupabaseProvider::isAuthenticated() const
{
    return !_accessToken.empty() && !_userId.empty();
}

bool SupabaseProvider::loginWithDefaults()
{
    auto [username, password] = defaultLoginPassword();

    if (username.empty() || password.empty()) {
        P_LOG_WARNING("No default credentials available");
        return false;
    }

    return login(username, password);
}

std::pair<std::string, std::string> SupabaseProvider::defaultLoginPassword() const
{
    static const std::string username = pointless::getenv_or_empty("POINTLESS_USERNAME");
    static const std::string password = pointless::getenv_or_empty("POINTLESS_PASSWORD");

    if (username.empty() || password.empty()) {
        pointless::abort("Environment variables POINTLESS_USERNAME and POINTLESS_PASSWORD must be set for this test.");
    }

    return { username, password };
}

void SupabaseProvider::logout()
{
    _accessToken.clear();
    _userId.clear();
}

bool SupabaseProvider::pushData(const std::string &data)
{
    if (!isAuthenticated()) {
        P_LOG_ERROR("Cannot update data: not authenticated");
        return false;
    }

    auto compressed_bytes = compress(data);
    auto base64ed = base64Encode(compressed_bytes);

    const std::string full_url = "https://" + _baseUrl + "/rest/v1/Documents";
    const std::string body = R"({"data":")" + base64ed + R"(","id":0})";

    auto response = cpr::Post(
        cpr::Url { full_url },
        cpr::Header {
            { "apikey", _anonKey },
            { "Authorization", "Bearer " + _accessToken },
            { "Content-Type", "application/json" },
            { "Prefer", "return=minimal,resolution=merge-duplicates" } },
        cpr::Body { body },
        cpr::VerifySsl { shouldVerifySsl() });

    if (response.status_code != kHttpOk && response.status_code != kHttpCreated && response.status_code != kHttpNoContent) {
        P_LOG_ERROR("Failed to update data: HTTP {}", response.status_code);
        P_LOG_DEBUG("Response: {}", response.text);
        return false;
    }

    return true;
}

std::string SupabaseProvider::pullData()
{
    auto raw_data = retrieveRawData();
    if (raw_data.empty()) {
        return {};
    }

    auto compressed_bytes = base64Decode(raw_data);
    return decompress(compressed_bytes);
}

std::string SupabaseProvider::retrieveRawData()
{
    if (!isAuthenticated()) {
        P_LOG_ERROR("Cannot retrieve data: not authenticated");
        return {};
    }

    const std::string full_url = "https://" + _baseUrl + "/rest/v1/Documents";

    auto response = cpr::Get(
        cpr::Url { full_url },
        cpr::Parameters { { "select", "data" } },
        cpr::Header {
            { "apikey", _anonKey },
            { "Authorization", "Bearer " + _accessToken } },
        cpr::VerifySsl { shouldVerifySsl() });

    if (response.status_code != kHttpOk) {
        P_LOG_ERROR("HTTP request failed with status: {}", response.status_code);
        P_LOG_DEBUG("Response: {}", response.text);
        return {};
    }

    auto json_result = glz::read_json<glz::json_t>(response.text);
    if (!json_result.has_value()) {
        P_LOG_ERROR("Failed to parse JSON response");
        return {};
    }

    auto &json_obj = json_result.value();
    if (!json_obj.is_array() || json_obj.get_array().empty()) {
        P_LOG_WARNING("Response is empty or not an array");
        return {};
    }

    auto &first_item = json_obj.get_array()[0];
    if (!first_item.is_object()) {
        P_LOG_ERROR("First item in response is not an object");
        return {};
    }

    auto data_it = first_item.get_object().find("data");
    if (data_it == first_item.get_object().end() || !data_it->second.is_string()) {
        P_LOG_WARNING("No 'data' field found in response");
        return {};
    }

    std::string data = data_it->second.get_string();


    return data;
}

std::vector<uint8_t> SupabaseProvider::compress(const std::string &data)
{
    z_stream zs {};
    if (deflateInit2(&zs, Z_DEFAULT_COMPRESSION, Z_DEFLATED, MAX_WBITS + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
        throw std::runtime_error("Failed to initialize zlib deflation");
    }

    std::vector<Bytef> inputBuffer(data.begin(), data.end());
    zs.next_in = inputBuffer.data();
    zs.avail_in = static_cast<uInt>(data.length());

    int ret = 0;
    std::array<Bytef, kBufferSize> outbuffer {};
    std::vector<uint8_t> result;

    zs.next_out = outbuffer.data();
    zs.avail_out = static_cast<uInt>(outbuffer.size());
    ret = deflate(&zs, Z_FINISH);
    while (ret == Z_OK) {
        if (result.size() < zs.total_out) {
            result.insert(result.end(),
                          outbuffer.begin(),
                          outbuffer.begin() + static_cast<ptrdiff_t>(zs.total_out - result.size()));
        }
        zs.next_out = outbuffer.data();
        zs.avail_out = static_cast<uInt>(outbuffer.size());
        ret = deflate(&zs, Z_FINISH);
    }

    if (result.size() < zs.total_out) {
        result.insert(result.end(),
                      outbuffer.begin(),
                      outbuffer.begin() + static_cast<ptrdiff_t>(zs.total_out - result.size()));
    }

    deflateEnd(&zs);

    if (ret != Z_STREAM_END) {
        throw std::runtime_error("Failed to compress gzip data");
    }

    return result;
}

std::string SupabaseProvider::decompress(const std::vector<uint8_t> &compressed_data)
{
    z_stream zs {};
    if (inflateInit2(&zs, MAX_WBITS + 16) != Z_OK) {
        throw std::runtime_error("Failed to initialize zlib inflation");
    }

    std::vector<Bytef> inputBuffer(compressed_data.begin(), compressed_data.end());
    zs.next_in = inputBuffer.data();
    zs.avail_in = static_cast<uInt>(compressed_data.size());

    int ret = 0;
    std::array<char, kBufferSize> outbuffer {};
    std::string result;

    zs.next_out = reinterpret_cast<Bytef *>(outbuffer.data()); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
    zs.avail_out = static_cast<uInt>(outbuffer.size());
    ret = inflate(&zs, 0);
    while (ret == Z_OK) {
        if (result.size() < zs.total_out) {
            result.append(outbuffer.data(), zs.total_out - result.size());
        }
        zs.next_out = reinterpret_cast<Bytef *>(outbuffer.data()); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        zs.avail_out = static_cast<uInt>(outbuffer.size());
        ret = inflate(&zs, 0);
    }

    if (result.size() < zs.total_out) {
        result.append(outbuffer.data(), zs.total_out - result.size());
    }

    inflateEnd(&zs);

    if (ret != Z_STREAM_END) {
        throw std::runtime_error("Failed to decompress gzip data");
    }

    return result;
}

std::vector<uint8_t> SupabaseProvider::base64Decode(const std::string &input)
{
    const std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::vector<uint8_t> result;

    uint32_t val = 0;
    int32_t valb = -8;
    for (unsigned char c : input) {
        auto pos = chars.find(static_cast<char>(c));
        if (pos == std::string::npos)
            break;
        val = (val << 6U) + static_cast<uint32_t>(pos);
        valb += 6;
        if (valb >= 0) {
            result.push_back(static_cast<uint8_t>((val >> static_cast<uint32_t>(valb)) & 0xFFU));
            valb -= 8;
        }
    }
    return result;
}

std::string SupabaseProvider::base64Encode(const std::vector<uint8_t> &data)
{
    const std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string result;
    uint32_t val = 0;
    int32_t valb = -6;

    for (uint8_t c : data) {
        val = (val << 8U) + c;
        valb += 8;
        while (valb >= 0) {
            result.push_back(chars[(val >> static_cast<uint32_t>(valb)) & 0x3FU]);
            valb -= 6;
        }
    }

    if (valb > -6) {
        result.push_back(chars[((val << 8U) >> static_cast<uint32_t>(valb + 8)) & 0x3FU]);
    }

    while ((result.size() % 4) != 0) {
        result.push_back('=');
    }

    return result;
}
