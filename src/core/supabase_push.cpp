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
        if (argc != 3) {
            std::cerr << "Usage: " << argv[0] << " [--test|--release] <json_file>\n";
            std::cerr << "  --test     Use test Supabase environment\n";
            std::cerr << "  --release  Use production Supabase environment\n";
            return 1;
        }

        std::string mode = argv[1];
        if (mode != "--test" && mode != "--release") {
            std::cerr << "Error: First argument must be --test or --release\n";
            std::cerr << "Usage: " << argv[0] << " [--test|--release] <json_file>\n";
            return 1;
        }

        core::Logger::initLogLevel();

        if (mode == "--test") {
            core::Context::setContext(core::Context::defaultContextForSupabaseTesting());
        } else {
            core::Context::setContext(core::Context::defaultContextForSupabaseRelease());
        }

        std::string filePath = argv[2];
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
