
// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "../core/supabase.h"
#include "../core/local_data.h"

#include <quill/Backend.h>
#include <quill/Frontend.h>
#include <quill/LogMacros.h>
#include <quill/sinks/ConsoleSink.h>

#include <iostream>
#include <string>


void supabase_test()
{
    try {
        auto supabase = Supabase::createDefault(true);

        if (!supabase.isAuthenticated()) {
            std::cout << "Failed to authenticate with Supabase" << std::endl;
            return;
        }

        std::cout << "Successfully authenticated with Supabase" << std::endl;

        std::string data = supabase.retrieveData();

        if (!data.empty()) {
            std::cout << "Retrieved and decompressed data:\n"
                      << data << std::endl;
        } else {
            std::cout << "No data retrieved" << std::endl;
        }
    } catch (const std::exception &e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
}

int main(int argc, char *argv[])
{
    quill::Backend::start();

    auto console_sink = quill::Frontend::create_or_get_sink<quill::ConsoleSink>("console_sink");
    auto logger = quill::Frontend::create_or_get_logger("pointless_cli", console_sink);

    LOG_INFO(logger, "Testing Supabase connection...");
    supabase_test();

    // try {
    //     LocalData localData;
    //     auto result = localData.loadTaskManager();
    //     if (!result) {
    //         LOG_ERROR(logger, "Error: {}", result.error());
    //         return 1;
    //     }

    //     const auto &manager = result.value();
    //     LOG_INFO(logger, "Loaded TaskManager successfully from local data");
    //     LOG_INFO(logger, "Task count: {}", manager.taskCount());
    //     for (const auto &task : manager.getAllTasks()) {
    //         LOG_INFO(logger, "Task: {}", task.title);
    //     }
    // } catch (const std::exception& e) {
    //     LOG_ERROR(logger, "Error initializing LocalData: {}", e.what());
    //     return 1;
    // }

    return 0;
}
