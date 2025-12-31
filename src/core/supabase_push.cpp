// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "context.h"
#include "data_provider.h"
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
        if (argc != 2) {
            std::cerr << "Usage: " << argv[0] << " <json_file>\n";
            return 1;
        }

        core::Logger::initLogLevel();
        core::Context::setContext(core::Context::defaultContextForSupabaseTesting());

        std::string filePath = argv[1];
        std::ifstream file(filePath);
        if (!file.is_open()) {
            P_LOG_ERROR("Failed to open file: {}", filePath);
            return 1;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string jsonData = buffer.str();

        auto provider = IDataProvider::createProvider();
        if (!provider) {
            P_LOG_ERROR("Failed to create data provider");
            return 1;
        }

        if (!provider->loginWithDefaults()) {
            P_LOG_ERROR("Failed to login");
            return 1;
        }

        if (provider->pushData(jsonData)) {
            P_LOG_INFO("Data pushed successfully");
        } else {
            P_LOG_ERROR("Failed to push data");
            return 1;
        }

        return 0;
    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "Unknown exception" << "\n";
        return 1;
    }
}
