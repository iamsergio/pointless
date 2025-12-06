// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "supabase.h"
#include "logger.h"

#include <cpr/cpr.h>
#include <cpr/error.h>
#include <zlib.h>

#include <stdexcept>

namespace {

bool shouldVerifySsl()
{
#if defined(POINTLESS_DEVELOPER_MODE) && defined(Q_OS_IOS)
    return false;
#else
    return true;
#endif
}

}


Supabase::Supabase(const std::string &base_url, const std::string &anon_key)
    : _baseUrl(base_url)
    , _anonKey(anon_key)
{
    const char *env_username = std::getenv("POINTLESS_USERNAME");
    const char *env_password = std::getenv("POINTLESS_PASSWORD");
    if (env_username) {
        _defaultUser = env_username;
    }

    if (env_password) {
        _defaultPassword = env_password;
    }
}

std::unique_ptr<Supabase> Supabase::createDefault()
{
#ifndef POINTLESS_SUPABASE_URL
#error "POINTLESS_SUPABASE_URL is not defined"
#endif

#ifndef POINTLESS_SUPABASE_ANON_KEY
#error "POINTLESS_SUPABASE_ANON_KEY is not defined"
#endif

    return std::make_unique<Supabase>(POINTLESS_SUPABASE_URL, POINTLESS_SUPABASE_ANON_KEY);
}

bool Supabase::login(const std::string &email, const std::string &password)
{
    std::string auth_url = "https://" + _baseUrl + "/auth/v1/token?grant_type=password";
    std::string body = R"({"email":")" + email + R"(","password":")" + password + R"("})";

    auto response = cpr::Post(
        cpr::Url { auth_url },
        cpr::Header {
            { "apikey", _anonKey },
            { "Content-Type", "application/json" } },
        cpr::Body { body },
        cpr::VerifySsl { shouldVerifySsl() });

    if (response.status_code != 200) {
        LOG_ERROR(Logger::getLogger(), "Login failed: HTTP={} url={}", response.status_code, auth_url);
        LOG_ERROR(Logger::getLogger(), "Response: text={} cpr::ErrorCode={} error.msg={}", response.text, static_cast<int>(response.error.code), response.error.message);
#ifdef POINTLESS_DEVELOPER_MODE
        LOG_ERROR(Logger::getLogger(), "Request body: {}", body);
#endif
        return false;
    }

    auto json_result = glz::read_json<glz::json_t>(response.text);
    if (!json_result) {
        LOG_ERROR(Logger::getLogger(), "Failed to parse login response JSON");
        return false;
    }

    auto &json_obj = json_result.value();
    if (!json_obj.is_object()) {
        return false;
    }

    auto access_token_it = json_obj.get_object().find("access_token");
    if (access_token_it == json_obj.get_object().end() || !access_token_it->second.is_string()) {
        LOG_ERROR(Logger::getLogger(), "No access_token in login response");
        return false;
    }

    auto user_it = json_obj.get_object().find("user");
    if (user_it == json_obj.get_object().end() || !user_it->second.is_object()) {
        LOG_ERROR(Logger::getLogger(), "No user object in login response");
        return false;
    }

    auto &user_obj = user_it->second.get_object();
    auto id_it = user_obj.find("id");
    if (id_it == user_obj.end() || !id_it->second.is_string()) {
        LOG_ERROR(Logger::getLogger(), "No user id in login response");
        return false;
    }

    _accessToken = access_token_it->second.get_string();
    _userId = id_it->second.get_string();



    return true;
}

bool Supabase::isAuthenticated() const
{
    return !_accessToken.empty() && !_userId.empty();
}

bool Supabase::loginWithDefaults()
{
    if (_defaultUser.empty() || _defaultPassword.empty()) {
        LOG_WARNING(Logger::getLogger(), "No default credentials available");
        return false;
    }

    return login(_defaultUser, _defaultPassword);
}

std::pair<std::string, std::string> Supabase::defaultLoginPassword() const
{
    return { _defaultUser, _defaultPassword };
}

void Supabase::logout()
{
    _accessToken.clear();
    _userId.clear();
}

bool Supabase::updateData(const std::string &data)
{
    if (!isAuthenticated()) {
        LOG_ERROR(Logger::getLogger(), "Cannot update data: not authenticated");
        return false;
    }

    auto compressed_bytes = compress(data);
    auto base64ed = base64Encode(compressed_bytes);

    std::string full_url = "https://" + _baseUrl + "/rest/v1/Documents";
    std::string body = R"({"data":")" + base64ed + R"(","id":0})";

    auto response = cpr::Post(
        cpr::Url { full_url },
        cpr::Header {
            { "apikey", _anonKey },
            { "Authorization", "Bearer " + _accessToken },
            { "Content-Type", "application/json" },
            { "Prefer", "return=minimal,resolution=merge-duplicates" } },
        cpr::Body { body },
        cpr::VerifySsl { shouldVerifySsl() });

    if (response.status_code != 200 && response.status_code != 201 && response.status_code != 204) {
        LOG_ERROR(Logger::getLogger(), "Failed to update data: HTTP {}", response.status_code);
        LOG_DEBUG(Logger::getLogger(), "Response: {}", response.text);
        return false;
    }

    return true;
}

std::string Supabase::retrieveData()
{
    auto raw_data = retrieveRawData();
    if (raw_data.empty()) {
        return {};
    }

    auto compressed_bytes = base64Decode(raw_data);
    return decompress(compressed_bytes);
}

std::string Supabase::retrieveRawData()
{
    if (!isAuthenticated()) {
        LOG_ERROR(Logger::getLogger(), "Cannot retrieve data: not authenticated");
        return {};
    }

    std::string full_url = "https://" + _baseUrl + "/rest/v1/Documents";

    auto response = cpr::Get(
        cpr::Url { full_url },
        cpr::Parameters { { "select", "data" } },
        cpr::Header {
            { "apikey", _anonKey },
            { "Authorization", "Bearer " + _accessToken } },
        cpr::VerifySsl { shouldVerifySsl() });

    if (response.status_code != 200) {
        LOG_ERROR(Logger::getLogger(), "HTTP request failed with status: {}", response.status_code);
        LOG_DEBUG(Logger::getLogger(), "Response: {}", response.text);
        return {};
    }

    auto json_result = glz::read_json<glz::json_t>(response.text);
    if (!json_result) {
        LOG_ERROR(Logger::getLogger(), "Failed to parse JSON response");
        return {};
    }

    auto &json_obj = json_result.value();
    if (!json_obj.is_array() || json_obj.get_array().empty()) {
        LOG_WARNING(Logger::getLogger(), "Response is empty or not an array");
        return {};
    }

    auto &first_item = json_obj.get_array()[0];
    if (!first_item.is_object()) {
        LOG_ERROR(Logger::getLogger(), "First item in response is not an object");
        return {};
    }

    auto data_it = first_item.get_object().find("data");
    if (data_it == first_item.get_object().end() || !data_it->second.is_string()) {
        LOG_WARNING(Logger::getLogger(), "No 'data' field found in response");
        return {};
    }

    std::string data = data_it->second.get_string();


    return data;
}

std::vector<uint8_t> Supabase::compress(const std::string &data)
{
    z_stream zs {};
    if (deflateInit2(&zs, Z_DEFAULT_COMPRESSION, Z_DEFLATED, MAX_WBITS + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
        throw std::runtime_error("Failed to initialize zlib deflation");
    }

    zs.next_in = reinterpret_cast<Bytef *>(const_cast<char *>(data.c_str()));
    zs.avail_in = static_cast<uInt>(data.length());

    int ret;
    char outbuffer[32768];
    std::vector<uint8_t> result;

    do {
        zs.next_out = reinterpret_cast<Bytef *>(outbuffer);
        zs.avail_out = sizeof(outbuffer);

        ret = deflate(&zs, Z_FINISH);

        if (result.size() < zs.total_out) {
            result.insert(result.end(),
                          outbuffer,
                          outbuffer + zs.total_out - result.size());
        }
    } while (ret == Z_OK);

    deflateEnd(&zs);

    if (ret != Z_STREAM_END) {
        throw std::runtime_error("Failed to compress gzip data");
    }

    return result;
}

std::string Supabase::decompress(const std::vector<uint8_t> &compressed_data)
{
    z_stream zs {};
    if (inflateInit2(&zs, MAX_WBITS + 16) != Z_OK) {
        throw std::runtime_error("Failed to initialize zlib inflation");
    }

    zs.next_in = const_cast<Bytef *>(compressed_data.data());
    zs.avail_in = static_cast<uInt>(compressed_data.size());

    int ret;
    char outbuffer[32768];
    std::string result;

    do {
        zs.next_out = reinterpret_cast<Bytef *>(outbuffer);
        zs.avail_out = sizeof(outbuffer);

        ret = inflate(&zs, 0);

        if (result.size() < zs.total_out) {
            result.append(outbuffer, zs.total_out - result.size());
        }
    } while (ret == Z_OK);

    inflateEnd(&zs);

    if (ret != Z_STREAM_END) {
        throw std::runtime_error("Failed to decompress gzip data");
    }

    return result;
}

std::vector<uint8_t> Supabase::base64Decode(const std::string &input)
{
    const std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::vector<uint8_t> result;

    int val = 0, valb = -8;
    for (unsigned char c : input) {
        if (chars.find(c) == std::string::npos)
            break;
        val = (val << 6) + static_cast<int>(chars.find(c));
        valb += 6;
        if (valb >= 0) {
            result.push_back(static_cast<uint8_t>((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return result;
}

std::string Supabase::base64Encode(const std::vector<uint8_t> &data)
{
    const std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string result;
    int val = 0, valb = -6;

    for (uint8_t c : data) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            result.push_back(chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }

    if (valb > -6) {
        result.push_back(chars[((val << 8) >> (valb + 8)) & 0x3F]);
    }

    while (result.size() % 4) {
        result.push_back('=');
    }

    return result;
}
