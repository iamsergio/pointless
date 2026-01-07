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

bool TestLocalDataProvider::isAuthenticated() const
{
    return true;
}

std::string TestLocalDataProvider::pullData()
{
    std::ifstream file(_filePath);
    if (!file.is_open()) {
        P_LOG_ERROR("Failed to open file for reading: {}", _filePath);
        return {};
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool TestLocalDataProvider::pushData(const std::string &data)
{
    std::ofstream file(_filePath);
    if (!file.is_open()) {
        P_LOG_ERROR("Failed to open file for writing: {}", _filePath);
        return false;
    }
    file << data;
    return true;
}
