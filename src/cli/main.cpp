
// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "../core/supabase.h"
#include "../core/logger.h"

#include <string>


void supabase_test()
{
    try {
        auto supabase = Supabase::createDefault();

        if (!supabase.isAuthenticated()) {
            LOG_ERROR(Logger::getLogger(), "Failed to authenticate with Supabase");
            return;
        }

        LOG_INFO(Logger::getLogger(), "Successfully authenticated with Supabase");

        std::string data = supabase.retrieveData();

        if (!data.empty()) {
            LOG_INFO(Logger::getLogger(), "Retrieved and decompressed data:\n{}", data);
        } else {
            LOG_WARNING(Logger::getLogger(), "No data retrieved");
        }
    } catch (const std::exception &e) {
        LOG_ERROR(Logger::getLogger(), "Error: {}", e.what());
    }
}

int main(int argc, char *argv[])
{
    Logger::initialize();

    P_LOG_INFO("Testing Supabase connection...");
    supabase_test();

    // try {
    //     LocalData localData;
    //     auto result = localData.loadTaskManager();
    //     if (!result) {
    //         LOG_ERROR(Logger::getLogger(), "Error: {}", result.error());
    //         return 1;
    //     }

    //     const auto &manager = result.value();
    //     LOG_INFO(Logger::getLogger(), "Loaded TaskManager successfully from local data");
    //     LOG_INFO(Logger::getLogger(), "Task count: {}", manager.taskCount());
    //     for (const auto &task : manager.getAllTasks()) {
    //         LOG_INFO(Logger::getLogger(), "Task: {}", task.title);
    //     }
    // } catch (const std::exception& e) {
    //     LOG_ERROR(Logger::getLogger(), "Error initializing LocalData: {}", e.what());
    //     return 1;
    // }

    return 0;
}
