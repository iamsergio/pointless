# SPDX-FileCopyrightText: 2025 Sergio Martins
# SPDX-License-Identifier: MIT

find_package(nlohmann_json CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)


add_library(pointless_core SHARED
	pointless_core.cpp
	task.cpp
	task_manager.cpp
	tag.cpp
	date_utils.cpp
	item.cpp
)
target_link_libraries(pointless_core PUBLIC nlohmann_json::nlohmann_json PRIVATE quill::quill cpr::cpr)
target_include_directories(pointless_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


if (POINTLESS_ENABLE_TESTS)
	add_executable(test_date_utils tests/test_date_utils.cpp)
	target_link_libraries(test_date_utils PRIVATE pointless_core GTest::gtest_main)
	target_include_directories(test_date_utils PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
	add_test(NAME test_date_utils COMMAND test_date_utils)

	add_executable(test_taskmanager tests/test_taskmanager.cpp)
	target_link_libraries(test_taskmanager PRIVATE pointless_core GTest::gtest_main)
	target_include_directories(test_taskmanager PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
	target_compile_definitions(test_taskmanager PRIVATE POINTLESS_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
	add_test(NAME test_taskmanager COMMAND test_taskmanager)
endif()
