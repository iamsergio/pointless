// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "data.h"
#include "logger.h"

#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace pointless;

int main(int argc, char *argv[])
{
    try {
        if (argc != 4) {
            std::cerr << "Usage: " << argv[0] << " <json_file> <old_tag_name> <new_tag_name>\n";
            return 1;
        }

        core::Logger::initLogLevel();

        std::string filePath = argv[1];
        std::string oldName = argv[2];
        std::string newName = argv[3];

        std::ifstream file(filePath);
        if (!file.is_open()) {
            P_LOG_ERROR("Failed to open file: {}", filePath);
            return 1;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string jsonData = buffer.str();
        file.close();

        auto dataResult = core::Data::fromJson(jsonData);
        if (!dataResult) {
            P_LOG_ERROR("Failed to parse JSON: {}", dataResult.error());
            return 1;
        }

        auto &data = dataResult.value();

        if (!data.renameTag(oldName, newName)) {
            P_LOG_ERROR("Failed to rename tag '{}' to '{}'", oldName, newName);
            return 1;
        }

        auto jsonResult = data.toJson();
        if (!jsonResult) {
            P_LOG_ERROR("Failed to serialize JSON: {}", jsonResult.error());
            return 1;
        }

        std::ofstream outFile(filePath);
        if (!outFile.is_open()) {
            P_LOG_ERROR("Failed to open file for writing: {}", filePath);
            return 1;
        }

        outFile << jsonResult.value();
        P_LOG_INFO("Renamed tag '{}' to '{}' in {}", oldName, newName, filePath);

        return 0;
    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "Unknown exception" << "\n";
        return 1;
    }
}
