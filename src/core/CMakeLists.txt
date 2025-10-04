# SPDX-FileCopyrightText: 2025 Sergio Martins
# SPDX-License-Identifier: MIT

find_package(nlohmann_json CONFIG REQUIRED)

enable_testing()

add_library(pointless_core SHARED
	pointless_core.cpp
	task.cpp
	tag.cpp
	date_utils.cpp
)
target_link_libraries(pointless_core PRIVATE quill::quill nlohmann_json::nlohmann_json)
target_include_directories(pointless_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# GoogleTest
find_package(GTest CONFIG REQUIRED)
add_executable(test_date_utils tests/test_date_utils.cpp)
target_link_libraries(test_date_utils PRIVATE pointless_core GTest::gtest_main)
target_include_directories(test_date_utils PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME test_date_utils COMMAND test_date_utils)
