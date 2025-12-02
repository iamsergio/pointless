// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "file_data_provider.h"
#include "logger.h"

#include <fstream>
#include <sstream>

FileDataProvider::FileDataProvider(const std::string &filePath)
    : _filePath(filePath)
{
}

bool FileDataProvider::isAuthenticated() const
{
    return true;
}

std::string FileDataProvider::retrieveData()
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

bool FileDataProvider::updateData(const std::string &data)
{
    std::ofstream file(_filePath);
    if (!file.is_open()) {
        P_LOG_ERROR("Failed to open file for writing: {}", _filePath);
        return false;
    }
    file << data;
    return true;
}
