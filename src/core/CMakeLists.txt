# SPDX-FileCopyrightText: 2025 Sergio Martins
# SPDX-License-Identifier: MIT

add_library(pointless_core SHARED pointless_core.cpp)
target_link_libraries(pointless_core PRIVATE quill::quill)
target_include_directories(pointless_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
