// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "context.h"
#include "data_provider.h"
#include "logger.h"

#include <glaze/glaze.hpp>

#include <exception>
#include <iostream>
#include <string>

using namespace pointless;

int main(int argc, char *argv[])
{
    try {
        if (argc != 2) {
            std::cerr << "Usage: " << argv[0] << " [--test|--release]\n";
            std::cerr << "  --test     Use test Supabase environment\n";
            std::cerr << "  --release  Use production Supabase environment\n";
            return 1;
        }

        std::string mode = argv[1];
        if (mode != "--test" && mode != "--release") {
            std::cerr << "Error: First argument must be --test or --release\n";
            std::cerr << "Usage: " << argv[0] << " [--test|--release]\n";
            return 1;
        }

        core::Logger::initLogLevel();

        if (mode == "--test") {
            core::Context::setContext(core::Context::defaultContextForSupabaseTesting());
        } else {
            core::Context::setContext(core::Context::defaultContextForSupabaseRelease());
        }

        auto provider = IDataProvider::createProvider();
        if (!provider) {
            P_LOG_ERROR("Failed to create data provider");
            return 1;
        }

        if (!provider->loginWithDefaults()) {
            P_LOG_ERROR("Failed to login");
            return 1;
        }

        auto jsonDataResult = provider->pullData();
        if (!jsonDataResult) {
            P_LOG_ERROR("Failed to pull data: {}", std::string(jsonDataResult.error()));
            return 1;
        }

        std::string jsonData = *jsonDataResult;

        glz::json_t json {};
        auto error = glz::read_json(json, jsonData);
        if (error) {
            P_LOG_ERROR("Failed to parse JSON: {}", glz::format_error(error, jsonData));
            return 1;
        }

        const std::string prettyJson = glz::write<glz::opts { .prettify = true }>(json).value_or("");
        if (prettyJson.empty()) {
            P_LOG_ERROR("Failed to format JSON");
            return 1;
        }

        std::cout << prettyJson << "\n";

        return 0;
    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "Unknown exception" << "\n";
        return 1;
    }
}
