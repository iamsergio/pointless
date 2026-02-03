// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "test_local_provider.h"
#include "logger.h"

#include <fstream>
#include <sstream>
#include <string>

TestLocalDataProvider::TestLocalDataProvider(std::string filePath)
    : _filePath(std::move(filePath))
{
}

bool TestLocalDataProvider::login(const std::string & /*email*/, const std::string & /*password*/)
{
    return true;
}

bool TestLocalDataProvider::loginWithDefaults()
{
    return true;
}

std::pair<std::string, std::string> TestLocalDataProvider::defaultLoginPassword() const
{
    return {};
}

void TestLocalDataProvider::logout()
{
}

bool TestLocalDataProvider::isAuthenticated()
{
    return true;
}

std::expected<std::string, TraceableError> TestLocalDataProvider::pullData()
{
    std::ifstream file(_filePath);
    if (!file.is_open()) {
        return TraceableError::create("Failed to open file for reading: " + _filePath);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::expected<void, TraceableError> TestLocalDataProvider::pushData(const std::string &data)
{
    std::ofstream file(_filePath);
    if (!file.is_open()) {
        const std::string msg = "Failed to open file for writing: " + _filePath;
        P_LOG_ERROR("{}", msg);
        return TraceableError::create(msg);
    }
    file << data;
    return {};
}

std::string TestLocalDataProvider::accessToken() const
{
    return {};
}

std::string TestLocalDataProvider::refreshToken() const
{
    return {};
}

std::string TestLocalDataProvider::userId() const
{
    return {};
}

void TestLocalDataProvider::setAccessToken(const std::string & /*token*/)
{
}

void TestLocalDataProvider::setRefreshToken(const std::string & /*token*/)
{
}

void TestLocalDataProvider::setUserId(const std::string & /*userId*/)
{
}

bool TestLocalDataProvider::refreshAccessToken()
{
    return true;
}
