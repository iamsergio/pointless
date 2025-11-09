# SPDX-FileCopyrightText: 2025 Sergio Martins
#
# SPDX-License-Identifier: MIT

find_package(glaze CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)

add_library(pointless_core SHARED pointless_core.cpp task.cpp task_manager.cpp
                                  tag.cpp date_utils.cpp)

if(NOT DEFINED ENV{POINTLESS_SUPABASE_SERVICE_KEY})
  message(
    FATAL_ERROR "POINTLESS_SUPABASE_SERVICE_KEY environment variable is not set"
  )
endif()

if(NOT DEFINED ENV{POINTLESS_SUPABASE_URL})
  message(FATAL_ERROR "POINTLESS_SUPABASE_URL environment variable is not set")
endif()

target_compile_definitions(
  pointless_core
  PRIVATE POINTLESS_SUPABASE_SERVICE_KEY="$ENV{POINTLESS_SUPABASE_SERVICE_KEY}"
          POINTLESS_SUPABASE_URL="$ENV{POINTLESS_SUPABASE_URL}")

target_link_libraries(
  pointless_core
  PUBLIC glaze::glaze
  PRIVATE quill::quill cpr::cpr)
target_include_directories(pointless_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(POINTLESS_ENABLE_TESTS)
  add_executable(test_date_utils tests/test_date_utils.cpp)
  target_link_libraries(test_date_utils PRIVATE pointless_core
                                                GTest::gtest_main)
  target_include_directories(test_date_utils
                             PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  add_test(NAME test_date_utils COMMAND test_date_utils)

  add_executable(test_taskmanager tests/test_taskmanager.cpp)
  target_link_libraries(test_taskmanager PRIVATE pointless_core
                                                 GTest::gtest_main)
  target_include_directories(test_taskmanager
                             PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  target_compile_definitions(test_taskmanager
                             PRIVATE POINTLESS_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
  add_test(NAME test_taskmanager COMMAND test_taskmanager)

  add_executable(test_tag tests/test_tag.cpp)
  target_link_libraries(test_tag PRIVATE pointless_core GTest::gtest_main)
  target_include_directories(test_tag PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  add_test(NAME test_tag COMMAND test_tag)

  add_executable(test_task tests/test_task.cpp)
  target_link_libraries(test_task PRIVATE pointless_core GTest::gtest_main)
  target_include_directories(test_task PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  add_test(NAME test_task COMMAND test_task)
endif()
