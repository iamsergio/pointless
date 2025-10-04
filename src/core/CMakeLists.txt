# SPDX-FileCopyrightText: 2025 Sergio Martins
# SPDX-License-Identifier: MIT

find_package(nlohmann_json CONFIG REQUIRED)

add_library(pointless_core SHARED
	pointless_core.cpp
	task.cpp
	tag.cpp
)
target_link_libraries(pointless_core PRIVATE quill::quill nlohmann_json::nlohmann_json)
target_include_directories(pointless_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
