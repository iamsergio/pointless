# SPDX-FileCopyrightText: 2025 Sergio Martins
#
# SPDX-License-Identifier: MIT

find_package(glaze CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)
find_package(ZLIB REQUIRED)

if(POINTLESS_ENABLE_TESTS)
  set(POINTLESS_TESTS_SRCS test_local_provider.cpp test_supabase_provider.cpp)
endif()

add_library(
  pointless_core STATIC
  pointless_core.cpp
  Clock.cpp
  context.cpp
  task.cpp
  data.cpp
  tag.cpp
  date_utils.cpp
  supabase.cpp
  data_provider.cpp
  local_data.cpp
  merger.cpp
  ${POINTLESS_TESTS_SRCS}
  logger.cpp
  calendar_provider.cpp)

if("$ENV{POINTLESS_SUPABASE_ANON_KEY}" STREQUAL "")
  message(FATAL_ERROR "POINTLESS_SUPABASE_ANON_KEY environment variable is empty or not set")
endif()

if("$ENV{POINTLESS_SUPABASE_URL}" STREQUAL "")
  message(FATAL_ERROR "POINTLESS_SUPABASE_URL environment variable is empty or not set")
endif()

target_compile_definitions(pointless_core PRIVATE POINTLESS_SUPABASE_URL="$ENV{POINTLESS_SUPABASE_URL}" POINTLESS_SUPABASE_ANON_KEY="$ENV{POINTLESS_SUPABASE_ANON_KEY}")

if(NOT "$ENV{POINTLESS_USERNAME}" STREQUAL "")
  target_compile_definitions(pointless_core PRIVATE POINTLESS_USERNAME="$ENV{POINTLESS_USERNAME}")
endif()

target_link_libraries(
  pointless_core
  PUBLIC glaze::glaze
  PRIVATE spdlog::spdlog cpr::cpr ZLIB::ZLIB)

if(APPLE)
  find_library(EVENTKIT_LIB EventKit REQUIRED)
  find_library(COREGRAPHICS_LIB CoreGraphics REQUIRED)
  target_link_libraries(pointless_core PRIVATE ${EVENTKIT_LIB} ${COREGRAPHICS_LIB})
  target_sources(pointless_core PRIVATE apple_calendar_provider.mm)
endif()

if(APPLE AND NOT IOS)
  add_executable(calendar_info calendar_info.cpp)
  target_link_libraries(calendar_info PRIVATE pointless_core)

  set_target_properties(
    calendar_info
    PROPERTIES MACOSX_BUNDLE TRUE
               MACOSX_BUNDLE_GUI_IDENTIFIER "com.iamsergio.pointless.calendar-info"
               MACOSX_BUNDLE_BUNDLE_NAME "CalendarInfo"
               MACOSX_BUNDLE_INFO_PLIST "${CMAKE_BINARY_DIR}/Info.plist")
  target_sources(calendar_info PRIVATE ${CMAKE_SOURCE_DIR}/src/mac/pointless.entitlements)
  set_source_files_properties("${CMAKE_SOURCE_DIR}/src/mac/pointless.entitlements" PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
endif()

target_include_directories(
  pointless_core
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../)

if(POINTLESS_ENABLE_TESTS)
  add_executable(test_date_utils tests/test_date_utils.cpp)
  target_link_libraries(test_date_utils PRIVATE pointless_core GTest::gtest_main)
  target_include_directories(test_date_utils PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  add_test(NAME test_date_utils COMMAND test_date_utils)

  add_executable(test_taskmanager tests/test_taskmanager.cpp)
  target_link_libraries(test_taskmanager PRIVATE pointless_core GTest::gtest_main)
  target_include_directories(test_taskmanager PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  target_compile_definitions(test_taskmanager PRIVATE POINTLESS_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
  add_test(NAME test_taskmanager COMMAND test_taskmanager)

  add_executable(test_tag tests/test_tag.cpp)
  target_link_libraries(test_tag PRIVATE pointless_core GTest::gtest_main)
  target_include_directories(test_tag PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  add_test(NAME test_tag COMMAND test_tag)

  add_executable(test_task tests/test_task.cpp)
  target_link_libraries(test_task PRIVATE pointless_core GTest::gtest_main)
  target_include_directories(test_task PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  add_test(NAME test_task COMMAND test_task)

  add_executable(test_supabase tests/test_supabase.cpp)
  target_link_libraries(test_supabase PRIVATE pointless_core GTest::gtest_main)
  target_include_directories(test_supabase PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  add_test(NAME test_supabase COMMAND test_supabase)

  add_executable(test_local_data tests/test_local_data.cpp)
  target_link_libraries(test_local_data PRIVATE pointless_core GTest::gtest_main)
  target_include_directories(test_local_data PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  add_test(NAME test_local_data COMMAND test_local_data)

  add_executable(test_data tests/test_data.cpp)
  target_link_libraries(test_data PRIVATE pointless_core GTest::gtest_main)
  target_include_directories(test_data PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  add_test(NAME test_data COMMAND test_data)
endif()

if(NOT IOS)
  add_executable(supabase_push supabase_push.cpp)
  target_link_libraries(supabase_push PRIVATE pointless_core)

  add_executable(supabase_pull supabase_pull.cpp)
  target_link_libraries(supabase_pull PRIVATE pointless_core)
endif()
